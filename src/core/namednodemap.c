/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/core/element.h>
#include <dom/core/node.h>
#include <dom/core/string.h>

#include "core/document.h"
#include "core/element.h"
#include "core/namednodemap.h"
#include "core/node.h"

#include "utils/utils.h"

/**
 * DOM named node map
 */
struct dom_namednodemap {
	struct dom_document *owner;	/**< Owning document */

	struct dom_node *head;		/**< Start of item list */

	dom_node_type type;		/**< Type of items in map */

	uint32_t refcnt;		/**< Reference count */
};

/**
 * Create a namednodemap
 *
 * \param doc   The owning document
 * \param head  Start of list containing items in map
 * \param type  The type of items in the map
 * \param map   Pointer to location to receive created map
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * ::head must be a node owned by ::doc and must be either an Element or
 * DocumentType node.
 *
 * If ::head is of type Element, ::type must be DOM_ATTRIBUTE_NODE
 * If ::head is of type DocumentType, ::type may be either
 * DOM_ENTITY_NODE or DOM_NOTATION_NODE.
 *
 * The returned map will already be referenced, so the client need not
 * explicitly reference it. The client must unref the map once it is
 * finished with it.
 */
dom_exception dom_namednodemap_create(struct dom_document *doc,
		struct dom_node *head, dom_node_type type,
		struct dom_namednodemap **map)
{
	struct dom_namednodemap *m;

	m = dom_document_alloc(doc, NULL, sizeof(struct dom_namednodemap));
	if (m == NULL)
		return DOM_NO_MEM_ERR;

	dom_node_ref((struct dom_node *) doc);
	m->owner = doc;

	dom_node_ref(head);
	m->head = head;

	m->type = type;

	m->refcnt = 1;

	*map = m;

	return DOM_NO_ERR;
}

/**
 * Claim a reference on a DOM named node map
 *
 * \param map  The map to claim a reference on
 */
void dom_namednodemap_ref(struct dom_namednodemap *map)
{
	map->refcnt++;
}

/**
 * Release a reference on a DOM named node map
 *
 * \param map  The map to release the reference from
 *
 * If the reference count reaches zero, any memory claimed by the
 * map will be released
 */
void dom_namednodemap_unref(struct dom_namednodemap *map)
{
	if (--map->refcnt == 0) {
		struct dom_node *owner = (struct dom_node *) map->owner;

		dom_node_unref(map->head);

		/* Remove map from document */
		dom_document_remove_namednodemap(map->owner, map);

		/* Destroy the map object */
		dom_document_alloc(map->owner, map, 0);

		/* And release our reference on the owning document
		 * This must be last as, otherwise, it's possible that
		 * the document is destroyed before we are */
		dom_node_unref(owner);
	}
}

/**
 * Retrieve the length of a named node map
 *
 * \param map     Map to retrieve length of
 * \param length  Pointer to location to receive length
 * \return DOM_NO_ERR.
 */
dom_exception dom_namednodemap_get_length(struct dom_namednodemap *map,
		unsigned long *length)
{
	struct dom_node *cur;
	unsigned long len = 0;

	switch (map->type) {
	case DOM_ATTRIBUTE_NODE:
		cur = dom_element_get_first_attribute(
				(struct dom_element *) map->head);
		break;
	case DOM_NOTATION_NODE:
	case DOM_ENTITY_NODE:
		/** \todo handle notation and entity nodes */
	default:
		return DOM_NOT_SUPPORTED_ERR;
		break;
	}

	for (; cur != NULL; cur = cur->next) {
		len++;
	}

	*length = len;

	return DOM_NO_ERR;
}

/**
 * Retrieve an item by name from a named node map
 *
 * \param map   The map to retrieve the item from
 * \param name  The name of the item to retrieve
 * \param node  Pointer to location to receive item
 * \return DOM_NO_ERR.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception dom_namednodemap_get_named_item(struct dom_namednodemap *map,
		struct dom_string *name, struct dom_node **node)
{
	struct dom_node *cur;

	switch (map->type) {
	case DOM_ATTRIBUTE_NODE:
		cur = dom_element_get_first_attribute(
				(struct dom_element *) map->head);
		break;
	case DOM_NOTATION_NODE:
	case DOM_ENTITY_NODE:
		/** \todo handle notation and entity nodes */
	default:
		return DOM_NOT_SUPPORTED_ERR;
		break;
	}

	for (; cur != NULL; cur = cur->next) {
		if (dom_string_cmp(cur->name, name) == 0) {
			break;
		}
	}

	if (cur != NULL) {
		dom_node_ref(cur);
	}
	*node = cur;

	return DOM_NO_ERR;
}

/**
 * Add a node to a named node map, replacing any matching existing node
 *
 * \param map   The map to add to
 * \param arg   The node to add
 * \param node  Pointer to location to receive replaced node
 * \return DOM_NO_ERR                      on success,
 *         DOM_WRONG_DOCUMENT_ERR          if ::arg was created from a
 *                                         different document than ::map,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::map is readonly,
 *         DOM_INUSE_ATTRIBUTE_ERR         if ::arg is an Attr that is
 *                                         already an attribute on another
 *                                         Element,
 *         DOM_HIERARCHY_REQUEST_ERR       if the type of ::arg is not
 *                                         permitted as a member of ::map.
 *
 * ::arg's nodeName attribute will be used to store it in ::map. It will
 * be accessible using the nodeName attribute as the key for lookup.
 *
 * Replacing a node by itself has no effect.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception dom_namednodemap_set_named_item(struct dom_namednodemap *map,
		struct dom_node *arg, struct dom_node **node)
{
	dom_exception err;

	/* Ensure arg and map belong to the same document */
	if (arg->owner != map->owner)
		return DOM_WRONG_DOCUMENT_ERR;

	/* Ensure map is writable */
	if (_dom_node_readonly(map->head))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Ensure arg isn't attached to another element */
	if (arg->type == DOM_ATTRIBUTE_NODE && arg->parent != NULL && 
			arg->parent != map->head)
		return DOM_INUSE_ATTRIBUTE_ERR;

	/* Ensure arg is permitted in the map */
	if (arg->type != map->type)
		return DOM_HIERARCHY_REQUEST_ERR;

	/* Now delegate to the container-specific function. 
	 * NamedNodeMaps are live, so this is fine. */
	switch (map->type) {
	case DOM_ATTRIBUTE_NODE:
		err = dom_element_set_attribute_node(
				(struct dom_element *) map->head, 
				(struct dom_attr *) arg, 
				(struct dom_attr **) node);
		break;
	case DOM_NOTATION_NODE:
	case DOM_ENTITY_NODE:
		/** \todo handle notation and entity nodes */
	default:
		err = DOM_NOT_SUPPORTED_ERR;
		break;
	}

	/* Reference counting is handled by the container-specific call */

	return err;
}

/**
 * Remove an item by name from a named node map
 *
 * \param map   The map to remove from
 * \param name  The name of the item to remove
 * \param node  Pointer to location to receive removed item
 * \return DOM_NO_ERR                      on success,
 *         DOM_NOT_FOUND_ERR               if there is no node named ::name
 *                                         in ::map,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::map is readonly.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception dom_namednodemap_remove_named_item(
		struct dom_namednodemap *map, struct dom_string *name,
		struct dom_node **node)
{
	dom_exception err;

	/* Ensure map is writable */
	if (_dom_node_readonly(map->head))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Now delegate to the container-specific function. 
	 * NamedNodeMaps are live, so this is fine. */
	switch (map->type) {
	case DOM_ATTRIBUTE_NODE:
	{
		struct dom_attr *attr;

		err = dom_element_get_attribute_node(
				(struct dom_element *) map->head,
				name, &attr);
		if (err == DOM_NO_ERR) {
			err = dom_element_remove_attribute_node(
				(struct dom_element *) map->head, 
				attr, (struct dom_attr **) node);
			if (err == DOM_NO_ERR) {
				/* No longer want attr */
				dom_node_unref((struct dom_node *) attr);
			}
		}
	}
		break;
	case DOM_NOTATION_NODE:
	case DOM_ENTITY_NODE:
		/** \todo handle notation and entity nodes */
	default:
		err = DOM_NOT_SUPPORTED_ERR;
		break;
	}

	/* Reference counting is handled by the container-specific call */

	return err;
}

/**
 * Retrieve an item from a named node map
 *
 * \param map    The map to retrieve the item from
 * \param index  The map index to retrieve
 * \param node   Pointer to location to receive item
 * \return DOM_NO_ERR.
 *
 * ::index is a zero-based index into ::map.
 * ::index lies in the range [0, length-1]
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception dom_namednodemap_item(struct dom_namednodemap *map,
		unsigned long index, struct dom_node **node)
{
	struct dom_node *cur;
	unsigned long count = 0;

	switch (map->type) {
	case DOM_ATTRIBUTE_NODE:
		cur = dom_element_get_first_attribute(
				(struct dom_element *) map->head);
		break;
	case DOM_NOTATION_NODE:
	case DOM_ENTITY_NODE:
		/** \todo handle notation and entity nodes */
	default:
		return DOM_NOT_SUPPORTED_ERR;
		break;
	}

	for (; cur != NULL; cur = cur->next) {
		count++;

		if ((index + 1) == count) {
			break;
		}
	}

	if (cur != NULL) {
		dom_node_ref(cur);
	}
	*node = cur;

	return DOM_NO_ERR;
}

/**
 * Retrieve an item by namespace/localname from a named node map
 *
 * \param map        The map to retrieve the item from
 * \param namespace  The namespace URI of the item to retrieve
 * \param localname  The local name of the node to retrieve
 * \param node       Pointer to location to receive item
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support the
 *                               feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception dom_namednodemap_get_named_item_ns(
		struct dom_namednodemap *map, struct dom_string *namespace,
		struct dom_string *localname, struct dom_node **node)
{
	struct dom_node *cur;

	/** \todo ensure XML feature is supported */

	switch (map->type) {
	case DOM_ATTRIBUTE_NODE:
		cur = dom_element_get_first_attribute(
				(struct dom_element *) map->head);
		break;
	case DOM_NOTATION_NODE:
	case DOM_ENTITY_NODE:
		/** \todo handle notation and entity nodes */
	default:
		return DOM_NOT_SUPPORTED_ERR;
		break;
	}

	for (; cur != NULL; cur = cur->next) {
		if (((namespace == NULL && cur->namespace == NULL) || 
			(namespace != NULL && 
			dom_string_cmp(cur->namespace, namespace) == 0)) &&
				dom_string_cmp(cur->name, localname) == 0) {
			break;
		}
	}

	if (cur != NULL) {
		dom_node_ref(cur);
	}
	*node = cur;

	return DOM_NO_ERR;
}

/**
 * Add a node to a named node map, replacing any matching existing node
 *
 * \param map   The map to add to
 * \param arg   The node to add
 * \param node  Pointer to location to receive replaced node
 * \return DOM_NO_ERR                      on success,
 *         DOM_WRONG_DOCUMENT_ERR          if ::arg was created from a
 *                                         different document than ::map,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::map is readonly,
 *         DOM_INUSE_ATTRIBUTE_ERR         if ::arg is an Attr that is
 *                                         already an attribute on another
 *                                         Element,
 *         DOM_HIERARCHY_REQUEST_ERR       if the type of ::arg is not
 *                                         permitted as a member of ::map.
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support the
 *                               feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * ::arg's namespaceURI and localName attributes will be used to store it in
 * ::map. It will be accessible using the namespaceURI and localName
 * attributes as the keys for lookup.
 *
 * Replacing a node by itself has no effect.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception dom_namednodemap_set_named_item_ns(
		struct dom_namednodemap *map, struct dom_node *arg,
		struct dom_node **node)
{
	dom_exception err;

	/** \todo ensure XML feature is supported */

	/* Ensure arg and map belong to the same document */
	if (arg->owner != map->owner)
		return DOM_WRONG_DOCUMENT_ERR;

	/* Ensure map is writable */
	if (_dom_node_readonly(map->head))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Ensure arg isn't attached to another element */
	if (arg->type == DOM_ATTRIBUTE_NODE && arg->parent != NULL && 
			arg->parent != map->head)
		return DOM_INUSE_ATTRIBUTE_ERR;

	/* Ensure arg is permitted in the map */
	if (arg->type != map->type)
		return DOM_HIERARCHY_REQUEST_ERR;

	/* Now delegate to the container-specific function. 
	 * NamedNodeMaps are live, so this is fine. */
	switch (map->type) {
	case DOM_ATTRIBUTE_NODE:
		err = dom_element_set_attribute_node_ns(
				(struct dom_element *) map->head, 
				(struct dom_attr *) arg, 
				(struct dom_attr **) node);
		break;
	case DOM_NOTATION_NODE:
	case DOM_ENTITY_NODE:
		/** \todo handle notation and entity nodes */
	default:
		err = DOM_NOT_SUPPORTED_ERR;
		break;
	}

	/* Reference counting is handled by the container-specific call */

	return err;
}

/**
 * Remove an item by namespace/localname from a named node map
 *
 * \param map        The map to remove from
 * \param namespace  The namespace URI of the item to remove
 * \param localname  The local name of the item to remove
 * \param node       Pointer to location to receive removed item
 * \return DOM_NO_ERR                      on success,
 *         DOM_NOT_FOUND_ERR               if there is no node named ::name
 *                                         in ::map,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::map is readonly.
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support the
 *                               feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * The returned node will have had its reference count increased. The client
 * should unref the node once it has finished with it.
 */
dom_exception dom_namednodemap_remove_named_item_ns(
		struct dom_namednodemap *map, struct dom_string *namespace,
		struct dom_string *localname, struct dom_node **node)
{
	dom_exception err;

	/** \todo ensure XML feature is supported */

	/* Ensure map is writable */
	if (_dom_node_readonly(map->head))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Now delegate to the container-specific function. 
	 * NamedNodeMaps are live, so this is fine. */
	switch (map->type) {
	case DOM_ATTRIBUTE_NODE:
	{
		struct dom_attr *attr;

		err = dom_element_get_attribute_node_ns(
				(struct dom_element *) map->head,
				namespace, localname, &attr);
		if (err == DOM_NO_ERR) {
			err = dom_element_remove_attribute_node(
				(struct dom_element *) map->head, 
				attr, (struct dom_attr **) node);
			if (err == DOM_NO_ERR) {
				/* No longer want attr */
				dom_node_unref((struct dom_node *) attr);
			}
		}
	}
		break;
	case DOM_NOTATION_NODE:
	case DOM_ENTITY_NODE:
		/** \todo handle notation and entity nodes */
	default:
		err = DOM_NOT_SUPPORTED_ERR;
		break;
	}

	/* Reference counting is handled by the container-specific call */

	return err;
}

/**
 * Match a namednodemap instance against a set of creation parameters
 *
 * \param map   The map to match
 * \param head  Start of list containing items in map
 * \param type  The type of items in the map
 * \return true if list matches, false otherwise
 */
bool dom_namednodemap_match(struct dom_namednodemap *map,
		struct dom_node *head, dom_node_type type)
{
	if (map->head == head && map->type == type)
		return true;

	return false;
}
