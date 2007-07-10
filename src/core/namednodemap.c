/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/core/node.h>

#include "core/document.h"
#include "core/namednodemap.h"

#include "utils/utils.h"

/**
 * DOM named node map
 */
struct dom_namednodemap {
	struct dom_document *owner;	/**< Owning document */

	struct dom_node *root;		/**< Node containing items in map */

	dom_namednodemap_type type;	/**< Type of map */

	uint32_t refcnt;		/**< Reference count */
};

/**
 * Create a namednodemap
 *
 * \param doc  The owning document
 * \param root  Node containing items in map
 * \param type  The type of map
 * \param map   Pointer to location to receive created map
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * ::root must be a node owned by ::doc and must be either an Element or
 * DocumentType node.
 *
 * If ::root is of type Element, ::type must be DOM_NAMEDNODEMAP_ATTRIBUTES
 * If ::root is of type DocumentType, ::type may be either
 * DOM_NAMEDNODEMAP_ENTITIES or DOM_NAMEDNODEMAP_NOTATIONS.
 *
 * The returned map will already be referenced, so the client need not
 * explicitly reference it. The client must unref the map once it is
 * finished with it.
 */
dom_exception dom_namednodemap_create(struct dom_document *doc,
		struct dom_node *root, dom_namednodemap_type type,
		struct dom_namednodemap **map)
{
	struct dom_namednodemap *m;

	m = dom_document_alloc(doc, NULL, sizeof(struct dom_namednodemap));
	if (m == NULL)
		return DOM_NO_MEM_ERR;

	dom_node_ref((struct dom_node *) doc);
	m->owner = doc;

	dom_node_ref(root);
	m->root = root;

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

		dom_node_unref(map->root);

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
	UNUSED(map);
	UNUSED(length);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(map);
	UNUSED(name);
	UNUSED(node);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(map);
	UNUSED(arg);
	UNUSED(node);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(map);
	UNUSED(name);
	UNUSED(node);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(map);
	UNUSED(index);
	UNUSED(node);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(map);
	UNUSED(namespace);
	UNUSED(localname);
	UNUSED(node);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(map);
	UNUSED(arg);
	UNUSED(node);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(map);
	UNUSED(namespace);
	UNUSED(localname);
	UNUSED(node);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Match a namednodemap instance against a set of creation parameters
 *
 * \param map  The map to match
 * \param root  Node containing items in map
 * \param type  The type of map
 * \return true if list matches, false otherwise
 */
bool dom_namednodemap_match(struct dom_namednodemap *map,
		struct dom_node *root, dom_namednodemap_type type)
{
	if (map->root == root && map->type == type)
		return true;

	return false;
}
