/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <assert.h>
#include <stdbool.h>

#include <dom/core/attr.h>
#include <dom/core/document.h>
#include <dom/core/string.h>

#include "core/attr.h"
#include "core/cdatasection.h"
#include "core/comment.h"
#include "core/document.h"
#include "core/document_type.h"
#include "core/doc_fragment.h"
#include "core/element.h"
#include "core/entity_ref.h"
#include "core/node.h"
#include "core/pi.h"
#include "core/text.h"
#include "utils/utils.h"

static bool _dom_node_permitted_child(const struct dom_node *parent, 
		const struct dom_node *child);
static inline void _dom_node_attach(struct dom_node *node, 
		struct dom_node *parent,
		struct dom_node *previous, 
		struct dom_node *next);
static inline void _dom_node_detach(struct dom_node *node);
static inline void _dom_node_attach_range(struct dom_node *first, 
		struct dom_node *last,
		struct dom_node *parent, 
		struct dom_node *previous, 
		struct dom_node *next);
static inline void _dom_node_detach_range(struct dom_node *first, 
		struct dom_node *last);
static inline void _dom_node_replace(struct dom_node *old, 
		struct dom_node *replacement);

/**
 * Destroy a DOM node
 *
 * \param node  The node to destroy
 *
 * ::node's parent link must be NULL and its reference count must be 0.
 *
 * ::node will be freed.
 *
 * This function should only be called from dom_node_unref or type-specific
 * destructors (for destroying child nodes). Anything else should not
 * be attempting to destroy nodes -- they should simply be unreferencing
 * them (so destruction will occur at the appropriate time).
 */
void dom_node_destroy(struct dom_node *node)
{
	struct dom_document *owner = node->owner;
	bool null_owner_permitted = (node->type == DOM_DOCUMENT_NODE || 
			node->type == DOM_DOCUMENT_TYPE_NODE);

	/* This function simply acts as a central despatcher
	 * for type-specific destructors. */

	assert(null_owner_permitted || owner != NULL); 

	if (!null_owner_permitted) {
		/* Claim a reference upon the owning document during 
		 * destruction to ensure that the document doesn't get 
		 * destroyed before its contents. */
		dom_node_ref((struct dom_node *) owner);
	}

	switch (node->type) {
	case DOM_ELEMENT_NODE:
		dom_element_destroy(owner, (struct dom_element *) node);
		break;
	case DOM_ATTRIBUTE_NODE:
		dom_attr_destroy(owner, (struct dom_attr *) node);
		break;
	case DOM_TEXT_NODE:
		dom_text_destroy(owner, (struct dom_text *) node);
		break;
	case DOM_CDATA_SECTION_NODE:
		dom_cdata_section_destroy(owner,
				(struct dom_cdata_section *) node);
		break;
	case DOM_ENTITY_REFERENCE_NODE:
		dom_entity_reference_destroy(owner,
				(struct dom_entity_reference *) node);
		break;
	case DOM_ENTITY_NODE:
		/** \todo entity node */
		break;
	case DOM_PROCESSING_INSTRUCTION_NODE:
		dom_processing_instruction_destroy(owner,
				(struct dom_processing_instruction *) node);
		break;
	case DOM_COMMENT_NODE:
		dom_comment_destroy(owner, (struct dom_comment *) node);
		break;
	case DOM_DOCUMENT_NODE:
		dom_document_destroy((struct dom_document *) node);
		break;
	case DOM_DOCUMENT_TYPE_NODE:
		dom_document_type_destroy((struct dom_document_type *) node);
		break;
	case DOM_DOCUMENT_FRAGMENT_NODE:
		dom_document_fragment_destroy(owner,
				(struct dom_document_fragment *) node);
		break;
	case DOM_NOTATION_NODE:
		/** \todo notation node */
		break;
	}

	if (!null_owner_permitted) {
		/* Release the reference we claimed on the document. If this 
		 * is the last reference held on the document and the list 
		 * of nodes pending deletion is empty, then the document will 
		 * be destroyed. */
		dom_node_unref((struct dom_node *) owner);
	}
}

/**
 * Initialise a DOM node
 *
 * \param node   The node to initialise
 * \param doc    The document which owns the node
 * \param type   The node type required
 * \param name   The node name, or NULL
 * \param value  The node value, or NULL
 * \return DOM_NO_ERR on success.
 *
 * ::name and ::value will have their reference counts increased.
 */
dom_exception dom_node_initialise(struct dom_node *node,
		struct dom_document *doc, dom_node_type type,
		struct dom_string *name, struct dom_string *value)
{
	if (name != NULL)
		dom_string_ref(name);
	node->name = name;

	if (value != NULL)
		dom_string_ref(value);
	node->value = value;

	node->type = type;

	node->parent = NULL;
	node->first_child = NULL;
	node->last_child = NULL;
	node->previous = NULL;
	node->next = NULL;
	node->attributes = NULL;

	/* Note: nodes do not reference the document to which they belong,
	 * as this would result in the document never being destroyed once
	 * the client has finished with it. The document will be aware of
	 * any nodes that it owns through 2 mechanisms:
	 *
	 * either a) Membership of the document tree
	 * or     b) Membership of the list of nodes pending deletion
	 *
	 * It is not possible for any given node to be a member of both
	 * data structures at the same time.
	 *
	 * The document will not be destroyed until both of these
	 * structures are empty. It will forcibly attempt to empty
	 * the document tree on document destruction. Any still-referenced
	 * nodes at that time will be added to the list of nodes pending
	 * deletion. This list will not be forcibly emptied, as it contains
	 * those nodes (and their sub-trees) in use by client code.
	 */
	node->owner = doc;

	/** \todo Namespace handling */
	node->namespace = NULL;
	node->prefix = NULL;
	node->localname = NULL;

	node->user_data = NULL;

	node->refcnt = 1;

	return DOM_NO_ERR;
}

/**
 * Finalise a DOM node
 *
 * \param doc   The owning document (or NULL if it's a standalone DocumentType)
 * \param node  The node to finalise
 *
 * The contents of ::node will be cleaned up. ::node will not be freed.
 * All children of ::node should have been removed prior to finalisation.
 */
void dom_node_finalise(struct dom_document *doc, struct dom_node *node)
{
	struct dom_user_data *u, *v;

	/* Standalone DocumentType nodes may not have user data attached */
	assert(node->type != DOM_DOCUMENT_TYPE_NODE || 
			node->user_data == NULL);

	/* Destroy user data */
	for (u = node->user_data; u != NULL; u = v) {
		v = u->next;

		dom_string_unref(u->key);

		dom_document_alloc(doc, u, 0);
	}

	if (node->localname != NULL)
		dom_string_unref(node->localname);

	if (node->prefix != NULL)
		dom_string_unref(node->prefix);

	if (node->namespace != NULL)
		dom_string_unref(node->namespace);

	/** \todo check if this node is in list of nodes pending deletion.
	 * If so, it must be removed from the list, so the document gets
	 * destroyed once the list is empty (and no longer referenced) */
	node->owner = NULL;

	/* Paranoia */
	node->attributes = NULL;
	node->next = NULL;
	node->previous = NULL;
	node->last_child = NULL;
	node->first_child = NULL;
	node->parent = NULL;

	if (node->value != NULL)
		dom_string_unref(node->value);

	if (node->name != NULL)
		dom_string_unref(node->name);
}

/**
 * Claim a reference on a DOM node
 *
 * \param node  The node to claim a reference on
 */
void dom_node_ref(struct dom_node *node)
{
	node->refcnt++;
}

/**
 * Release a reference on a DOM node
 *
 * \param node  The node to release the reference from
 *
 * If the reference count reaches zero and the node is not part of any
 * document, any memory claimed by the node will be released.
 */
void dom_node_unref(struct dom_node *node)
{
	if (node->refcnt > 0)
		node->refcnt--;

	if (node->refcnt == 0 && node->parent == NULL) {
		dom_node_destroy(node);
	}
}

/**
 * Retrieve the name of a DOM node
 *
 * \param node    The node to retrieve the name of
 * \param result  Pointer to location to receive node name
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_node_get_node_name(struct dom_node *node,
		struct dom_string **result)
{
	if (node->name != NULL)
		dom_string_ref(node->name);

	*result = node->name;

	return DOM_NO_ERR;
}

/**
 * Retrieve the value of a DOM node
 *
 * \param node    The node to retrieve the value of
 * \param result  Pointer to location to receive node value
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * DOM3Core states that this can raise DOMSTRING_SIZE_ERR. It will not in
 * this implementation; dom_strings are unbounded.
 */
dom_exception dom_node_get_node_value(struct dom_node *node,
		struct dom_string **result)
{
	if (node->type == DOM_ATTRIBUTE_NODE) {
		return dom_attr_get_value((struct dom_attr *) node, result);
	}

	if (node->value != NULL)
		dom_string_ref(node->value);

	*result = node->value;

	return DOM_NO_ERR;
}

/**
 * Set the value of a DOM node
 *
 * \param node   Node to set the value of
 * \param value  New value for node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if the node is readonly and the
 *                                         value is not defined to be null.
 *
 * The new value will have its reference count increased, so the caller
 * should unref it after the call (as the caller should have already claimed
 * a reference on the string). The node's existing value will be unrefed.
 */
dom_exception dom_node_set_node_value(struct dom_node *node,
		struct dom_string *value)
{
	/* This is a NOP if the value is defined to be null. */
	if (node->type == DOM_DOCUMENT_NODE || 
			node->type == DOM_DOCUMENT_FRAGMENT_NODE || 
			node->type == DOM_DOCUMENT_TYPE_NODE || 
			node->type == DOM_ELEMENT_NODE || 
			node->type == DOM_ENTITY_NODE || 
			node->type == DOM_ENTITY_REFERENCE_NODE || 
			node->type == DOM_NOTATION_NODE) {
		return DOM_NO_ERR;
	}

	/* Ensure node is writable */
	if (_dom_node_readonly(node))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* If it's an attribute node, then delegate setting to 
	 * the type-specific function */
	if (node->type == DOM_ATTRIBUTE_NODE)
		return dom_attr_set_value((struct dom_attr *) node, value);

	if (node->value != NULL)
		dom_string_unref(node->value);

	if (value != NULL)
		dom_string_ref(value);

	node->value = value;

	return DOM_NO_ERR;
}

/**
 * Retrieve the type of a DOM node
 *
 * \param node    The node to retrieve the type of
 * \param result  Pointer to location to receive node type
 * \return DOM_NO_ERR.
 */
dom_exception dom_node_get_node_type(struct dom_node *node, 
		dom_node_type *result)
{
	*result = node->type;

	return DOM_NO_ERR;
}

/**
 * Retrieve the parent of a DOM node
 *
 * \param node    The node to retrieve the parent of
 * \param result  Pointer to location to receive node parent
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_node_get_parent_node(struct dom_node *node,
		struct dom_node **result)
{
	/* Attr nodes have no parent */
	if (node->type == DOM_ATTRIBUTE_NODE) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	/* If there is a parent node, then increase its reference count */
	if (node->parent != NULL)
		dom_node_ref(node->parent);

	*result = node->parent;

	return DOM_NO_ERR;
}

/**
 * Retrieve a list of children of a DOM node
 *
 * \param node    The node to retrieve the children of
 * \param result  Pointer to location to receive child list
 * \return DOM_NO_ERR.
 *
 * The returned NodeList will be referenced. It is the responsibility
 * of the caller to unref the list once it has finished with it.
 */
dom_exception dom_node_get_child_nodes(struct dom_node *node,
		struct dom_nodelist **result)
{
	/* Can't do anything without an owning document.
	 * This is only a problem for DocumentType nodes 
	 * which are not yet attached to a document. 
	 * DocumentType nodes have no children, anyway. */
	if (node->owner == NULL)
		return DOM_NOT_SUPPORTED_ERR;

	return dom_document_get_nodelist(node->owner, node, 
			NULL, NULL, NULL, result);
}

/**
 * Retrieve the first child of a DOM node
 *
 * \param node    The node to retrieve the first child of
 * \param result  Pointer to location to receive node's first child
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_node_get_first_child(struct dom_node *node,
		struct dom_node **result)
{
	/* If there is a first child, increase its reference count */
	if (node->first_child != NULL)
		dom_node_ref(node->first_child);

	*result = node->first_child;

	return DOM_NO_ERR;
}

/**
 * Retrieve the last child of a DOM node
 *
 * \param node    The node to retrieve the last child of
 * \param result  Pointer to location to receive node's last child
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_node_get_last_child(struct dom_node *node,
		struct dom_node **result)
{
	/* If there is a last child, increase its reference count */
	if (node->last_child != NULL)
		dom_node_ref(node->last_child);

	*result = node->last_child;

	return DOM_NO_ERR;
}

/**
 * Retrieve the previous sibling of a DOM node
 *
 * \param node    The node to retrieve the previous sibling of
 * \param result  Pointer to location to receive node's previous sibling
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_node_get_previous_sibling(struct dom_node *node,
		struct dom_node **result)
{
	/* Attr nodes have no previous siblings */
	if (node->type == DOM_ATTRIBUTE_NODE) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	/* If there is a previous sibling, increase its reference count */
	if (node->previous != NULL)
		dom_node_ref(node->previous);

	*result = node->previous;

	return DOM_NO_ERR;
}

/**
 * Retrieve the subsequent sibling of a DOM node
 *
 * \param node    The node to retrieve the subsequent sibling of
 * \param result  Pointer to location to receive node's subsequent sibling
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_node_get_next_sibling(struct dom_node *node,
		struct dom_node **result)
{
	/* Attr nodes have no next siblings */
	if (node->type == DOM_ATTRIBUTE_NODE) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	/* If there is a subsequent sibling, increase its reference count */
	if (node->next != NULL)
		dom_node_ref(node->next);

	*result = node->next;

	return DOM_NO_ERR;
}

/**
 * Retrieve a map of attributes associated with a DOM node
 *
 * \param node    The node to retrieve the attributes of
 * \param result  Pointer to location to receive attribute map
 * \return DOM_NO_ERR.
 *
 * The returned NamedNodeMap will be referenced. It is the responsibility
 * of the caller to unref the map once it has finished with it.
 *
 * If ::node is not an Element, then NULL will be returned.
 */
dom_exception dom_node_get_attributes(struct dom_node *node,
		struct dom_namednodemap **result)
{
	if (node->type != DOM_ELEMENT_NODE) {
		*result = NULL;

		return DOM_NO_ERR;
	}

	return dom_document_get_namednodemap(node->owner, node, 
			DOM_ATTRIBUTE_NODE, result);
}

/**
 * Retrieve the owning document of a DOM node
 *
 * \param node    The node to retrieve the owner of
 * \param result  Pointer to location to receive node's owner
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_node_get_owner_document(struct dom_node *node,
		struct dom_document **result)
{
	/* Document nodes have no owner, as far as clients are concerned 
	 * In reality, they own themselves as this simplifies code elsewhere
	 */
	if (node->type == DOM_DOCUMENT_NODE) {
		*result = NULL;

		return DOM_NO_ERR;
	}

	/* If there is an owner, increase its reference count */
	if (node->owner != NULL)
		dom_node_ref((struct dom_node *) node->owner);

	*result = node->owner;

	return DOM_NO_ERR;
}

/**
 * Insert a child into a node
 *
 * \param node       Node to insert into
 * \param new_child  Node to insert
 * \param ref_child  Node to insert before, or NULL to insert as last child
 * \param result     Pointer to location to receive node being inserted
 * \return DOM_NO_ERR                      on success,
 *         DOM_HIERARCHY_REQUEST_ERR       if ::new_child's type is not
 *                                         permitted as a child of ::node,
 *                                         or ::new_child is an ancestor of
 *                                         ::node (or is ::node itself), or
 *                                         ::node is of type Document and a
 *                                         second DocumentType or Element is
 *                                         being inserted,
 *         DOM_WRONG_DOCUMENT_ERR          if ::new_child was created from a
 *                                         different document than ::node,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly, or
 *                                         ::new_child's parent is readonly,
 *         DOM_NOT_FOUND_ERR               if ::ref_child is not a child of
 *                                         ::node.
 *
 * If ::new_child is a DocumentFragment, all of its children are inserted.
 * If ::new_child is already in the tree, it is first removed.
 *
 * Attempting to insert a node before itself is a NOP.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_node_insert_before(struct dom_node *node,
		struct dom_node *new_child, struct dom_node *ref_child,
		struct dom_node **result)
{
	/* Ensure that new_child and node are owned by the same document */
	if ((new_child->type == DOM_DOCUMENT_TYPE_NODE && 
			new_child->owner != NULL && 
			new_child->owner != node->owner) ||
			(new_child->type != DOM_DOCUMENT_TYPE_NODE &&
			new_child->owner != node->owner))
		return DOM_WRONG_DOCUMENT_ERR;

	/* Ensure node isn't read only */
	if (_dom_node_readonly(node))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Ensure that ref_child (if any) is a child of node */
	if (ref_child != NULL && ref_child->parent != node)
		return DOM_NOT_FOUND_ERR;

	/* Ensure that new_child is not an ancestor of node, nor node itself */
	for (struct dom_node *n = node; n != NULL; n = n->parent) {
		if (n == new_child)
			return DOM_HIERARCHY_REQUEST_ERR;
	}

	/* Ensure that new_child is permitted as a child of node */
	if (!_dom_node_permitted_child(node, new_child))
		return DOM_HIERARCHY_REQUEST_ERR;

	/* DocumentType nodes are created outside the Document so, 
	 * if we're trying to attach a DocumentType node, then we
	 * also need to set its owner. */
	if (node->type == DOM_DOCUMENT_NODE &&
			new_child->type == DOM_DOCUMENT_TYPE_NODE) {
		/* See long comment in dom_node_initialise as to why 
		 * we don't ref the document here */
		new_child->owner = (struct dom_document *) node;
	}

	/* Attempting to insert a node before itself is a NOP */
	if (new_child == ref_child) {
		dom_node_ref(new_child);
		*result = new_child;

		return DOM_NO_ERR;
	}

	/* If new_child is already in the tree and 
	 * its parent isn't read only, remove it */
	if (new_child->parent != NULL) {
		if (_dom_node_readonly(new_child->parent))
			return DOM_NO_MODIFICATION_ALLOWED_ERR;

		_dom_node_detach(new_child);
	}

	/* If new_child is a DocumentFragment, insert its children 
	 * Otherwise, insert new_child */
	if (new_child->type == DOM_DOCUMENT_FRAGMENT_NODE) {
		if (new_child->first_child != NULL) {
			_dom_node_attach_range(new_child->first_child, 
					new_child->last_child, 
					node, 
					ref_child == NULL ? node->last_child 
							  : ref_child->previous,
					ref_child == NULL ? NULL 
							  : ref_child);

			new_child->first_child = NULL;
			new_child->last_child = NULL;
		}
	} else {
		_dom_node_attach(new_child, 
				node, 
				ref_child == NULL ? node->last_child 
						  : ref_child->previous, 
				ref_child == NULL ? NULL 
						  : ref_child);
	}

	/** \todo Is it correct to return DocumentFragments? */

	dom_node_ref(new_child);
	*result = new_child;

	return DOM_NO_ERR;
}

/**
 * Replace a node's child with a new one
 *
 * \param node       Node whose child to replace
 * \param new_child  Replacement node
 * \param old_child  Child to replace
 * \param result     Pointer to location to receive replaced node
 * \return DOM_NO_ERR                      on success,
 *         DOM_HIERARCHY_REQUEST_ERR       if ::new_child's type is not
 *                                         permitted as a child of ::node,
 *                                         or ::new_child is an ancestor of
 *                                         ::node (or is ::node itself), or
 *                                         ::node is of type Document and a
 *                                         second DocumentType or Element is
 *                                         being inserted,
 *         DOM_WRONG_DOCUMENT_ERR          if ::new_child was created from a
 *                                         different document than ::node,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly, or
 *                                         ::new_child's parent is readonly,
 *         DOM_NOT_FOUND_ERR               if ::old_child is not a child of
 *                                         ::node,
 *         DOM_NOT_SUPPORTED_ERR           if ::node is of type Document and
 *                                         ::new_child is of type
 *                                         DocumentType or Element.
 *
 * If ::new_child is a DocumentFragment, ::old_child is replaced by all of
 * ::new_child's children.
 * If ::new_child is already in the tree, it is first removed.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_node_replace_child(struct dom_node *node,
		struct dom_node *new_child, struct dom_node *old_child,
		struct dom_node **result)
{
	/* We don't support replacement of DocumentType or root Elements */
	if (node->type == DOM_DOCUMENT_NODE && 
			(new_child->type == DOM_DOCUMENT_TYPE_NODE || 
			new_child->type == DOM_ELEMENT_NODE))
		return DOM_NOT_SUPPORTED_ERR;

	/* Ensure that new_child and node are owned by the same document */
	if (new_child->owner != node->owner)
		return DOM_WRONG_DOCUMENT_ERR;

	/* Ensure node isn't read only */
	if (_dom_node_readonly(node))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Ensure that old_child is a child of node */
	if (old_child->parent != node)
		return DOM_NOT_FOUND_ERR;

	/* Ensure that new_child is not an ancestor of node, nor node itself */
	for (struct dom_node *n = node; n != NULL; n = n->parent) {
		if (n == new_child)
			return DOM_HIERARCHY_REQUEST_ERR;
	}

	/* Ensure that new_child is permitted as a child of node */
	if (!_dom_node_permitted_child(node, new_child))
		return DOM_HIERARCHY_REQUEST_ERR;

	/* Attempting to replace a node with itself is a NOP */
	if (new_child == old_child) {
		dom_node_ref(old_child);
		*result = old_child;

		return DOM_NO_ERR;
	}

	/* If new_child is already in the tree and 
	 * its parent isn't read only, remove it */
	if (new_child->parent != NULL) {
		if (_dom_node_readonly(new_child->parent))
			return DOM_NO_MODIFICATION_ALLOWED_ERR;

		_dom_node_detach(new_child);
	}

	/* Perform the replacement */
	_dom_node_replace(old_child, new_child);

	/* Sort out the return value */
	dom_node_ref(old_child);
	*result = old_child;

	return DOM_NO_ERR;
}

/**
 * Remove a child from a node
 *
 * \param node       Node whose child to replace
 * \param old_child  Child to remove
 * \param result     Pointer to location to receive removed node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly
 *         DOM_NOT_FOUND_ERR               if ::old_child is not a child of
 *                                         ::node,
 *         DOM_NOT_SUPPORTED_ERR           if ::node is of type Document and
 *                                         ::new_child is of type
 *                                         DocumentType or Element.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_node_remove_child(struct dom_node *node,
		struct dom_node *old_child,
		struct dom_node **result)
{
	/* We don't support removal of DocumentType or root Element nodes */
	if (node->type == DOM_DOCUMENT_NODE &&
			(old_child->type == DOM_DOCUMENT_TYPE_NODE ||
			old_child->type == DOM_ELEMENT_NODE))
		return DOM_NOT_SUPPORTED_ERR;

	/* Ensure old_child is a child of node */
	if (old_child->parent != node)
		return DOM_NOT_FOUND_ERR;

	/* Ensure node is writable */
	if (_dom_node_readonly(node))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Detach the node */
	_dom_node_detach(old_child);

	/* Sort out the return value */
	dom_node_ref(old_child);
	*result = old_child;

	return DOM_NO_ERR;
}

/**
 * Append a child to the end of a node's child list
 *
 * \param node       Node to insert into
 * \param new_child  Node to append
 * \param result     Pointer to location to receive node being inserted
 * \return DOM_NO_ERR                      on success,
 *         DOM_HIERARCHY_REQUEST_ERR       if ::new_child's type is not
 *                                         permitted as a child of ::node,
 *                                         or ::new_child is an ancestor of
 *                                         ::node (or is ::node itself), or
 *                                         ::node is of type Document and a
 *                                         second DocumentType or Element is
 *                                         being inserted,
 *         DOM_WRONG_DOCUMENT_ERR          if ::new_child was created from a
 *                                         different document than ::node,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly, or
 *                                         ::new_child's parent is readonly.
 *
 * If ::new_child is a DocumentFragment, all of its children are inserted.
 * If ::new_child is already in the tree, it is first removed.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_node_append_child(struct dom_node *node,
		struct dom_node *new_child,
		struct dom_node **result)
{
	/* This is just a veneer over insert_before */
	return dom_node_insert_before(node, new_child, NULL, result);
}

/**
 * Determine if a node has any children
 *
 * \param node    Node to inspect
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_node_has_child_nodes(struct dom_node *node, bool *result)
{
	*result = node->first_child != NULL;

	return DOM_NO_ERR;
}

/**
 * Clone a DOM node
 *
 * \param node    The node to clone
 * \param deep    True to deep-clone the node's sub-tree
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR        on success,
 *         DOM_NO_MEMORY_ERR on memory exhaustion.
 *
 * The returned node will already be referenced.
 *
 * The duplicate node will have no parent and no user data.
 *
 * If ::node has registered user_data_handlers, then they will be called.
 *
 * Cloning an Element copies all attributes & their values (including those
 * generated by the XML processor to represent defaulted attributes). It
 * does not copy any child nodes unless it is a deep copy (this includes
 * text contained within the Element, as the text is contained in a child
 * Text node).
 *
 * Cloning an Attr directly, as opposed to cloning as part of an Element,
 * returns a specified attribute. Cloning an Attr always clones its children,
 * since they represent its value, no matter whether this is a deep clone or
 * not.
 *
 * Cloning an EntityReference automatically constructs its subtree if a
 * corresponding Entity is available, no matter whether this is a deep clone
 * or not.
 *
 * Cloning any other type of node simply returns a copy.
 *
 * Note that cloning an immutable subtree results in a mutable copy, but
 * the children of an EntityReference clone are readonly. In addition, clones
 * of unspecified Attr nodes are specified.
 *
 * \todo work out what happens when cloning Document, DocumentType, Entity
 * and Notation nodes.
 */
dom_exception dom_node_clone_node(struct dom_node *node, bool deep,
		struct dom_node **result)
{
	UNUSED(node);
	UNUSED(deep);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Normalize a DOM node
 *
 * \param node  The node to normalize
 * \return DOM_NO_ERR.
 *
 * Puts all Text nodes in the full depth of the sub-tree beneath ::node,
 * including Attr nodes into "normal" form, where only structure separates
 * Text nodes.
 */
dom_exception dom_node_normalize(struct dom_node *node)
{
	UNUSED(node);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Test whether the DOM implementation implements a specific feature and
 * that feature is supported by the node.
 *
 * \param node     The node to test
 * \param feature  The name of the feature to test
 * \param version  The version number of the feature to test
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_node_is_supported(struct dom_node *node,
		struct dom_string *feature, struct dom_node *version,
		bool *result)
{
	UNUSED(node);
	UNUSED(feature);
	UNUSED(version);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the namespace of a DOM node
 *
 * \param node    The node to retrieve the namespace of
 * \param result  Pointer to location to receive node's namespace
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_node_get_namespace(struct dom_node *node,
		struct dom_string **result)
{
	/* If there is a namespace, increase its reference count */
	if (node->namespace != NULL)
		dom_string_ref(node->namespace);

	*result = node->namespace;

	return DOM_NO_ERR;
}

/**
 * Retrieve the prefix of a DOM node
 *
 * \param node    The node to retrieve the prefix of
 * \param result  Pointer to location to receive node's prefix
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_node_get_prefix(struct dom_node *node,
		struct dom_string **result)
{
	/* If there is a prefix, increase its reference count */
	if (node->prefix != NULL)
		dom_string_ref(node->prefix);

	*result = node->prefix;

	return DOM_NO_ERR;
}

/**
 * Set the prefix of a DOM node
 *
 * \param node    The node to set the prefix of
 * \param prefix  Pointer to prefix string
 * \return DOM_NO_ERR                      on success,
 *         DOM_INVALID_CHARACTER_ERR       if the specified prefix contains
 *                                         an illegal character,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly,
 *         DOM_NAMESPACE_ERR               if the specified prefix is
 *                                         malformed, if the namespaceURI of
 *                                         ::node is null, if the specified
 *                                         prefix is "xml" and the
 *                                         namespaceURI is different from
 *                                         "http://www.w3.org/XML/1998/namespace",
 *                                         if ::node is an attribute and the
 *                                         specified prefix is "xmlns" and
 *                                         the namespaceURI is different from
 *                                         "http://www.w3.org/2000/xmlns",
 *                                         or if this node is an attribute
 *                                         and the qualifiedName of ::node
 *                                         is "xmlns".
 */
dom_exception dom_node_set_prefix(struct dom_node *node,
		struct dom_string *prefix)
{
	UNUSED(node);
	UNUSED(prefix);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the local part of a node's qualified name
 *
 * \param node    The node to retrieve the local name of
 * \param result  Pointer to location to receive local name
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_node_get_local_name(struct dom_node *node,
		struct dom_string **result)
{
	/* If there is a local name, increase its reference count */
	if (node->localname != NULL)
		dom_string_ref(node->localname);

	*result = node->localname;

	return DOM_NO_ERR;
}

/**
 * Determine if a node has any attributes
 *
 * \param node    Node to inspect
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_node_has_attributes(struct dom_node *node, bool *result)
{
	*result = node->attributes != NULL;

	return DOM_NO_ERR;
}

/**
 * Retrieve the base URI of a DOM node
 *
 * \param node    The node to retrieve the base URI of
 * \param result  Pointer to location to receive base URI
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_node_get_base(struct dom_node *node,
		struct dom_string **result)
{
	UNUSED(node);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Compare the positions of two nodes in a DOM tree
 *
 * \param node   The reference node
 * \param other  The node to compare
 * \param result Pointer to location to receive result
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR when the nodes are from different DOM
 *                               implementations.
 *
 * The result is a bitfield of dom_document_position values.
 */
dom_exception dom_node_compare_document_position(struct dom_node *node,
		struct dom_node *other, uint16_t *result)
{
	UNUSED(node);
	UNUSED(other);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the text content of a DOM node
 *
 * \param node    The node to retrieve the text content of
 * \param result  Pointer to location to receive text content
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * DOM3Core states that this can raise DOMSTRING_SIZE_ERR. It will not in
 * this implementation; dom_strings are unbounded.
 */
dom_exception dom_node_get_text_content(struct dom_node *node,
		struct dom_string **result)
{
	UNUSED(node);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set the text content of a DOM node
 *
 * \param node     The node to set the text content of
 * \param content  New text content for node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly.
 *
 * Any child nodes ::node may have are removed and replaced with a single
 * Text node containing the new content.
 */
dom_exception dom_node_set_text_content(struct dom_node *node,
		struct dom_string *content)
{
	UNUSED(node);
	UNUSED(content);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Determine if two DOM nodes are the same
 *
 * \param node    The node to compare
 * \param other   The node to compare against
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * This tests if the two nodes reference the same object.
 */
dom_exception dom_node_is_same(struct dom_node *node, struct dom_node *other,
		bool *result)
{
	*result = (node == other);

	return DOM_NO_ERR;
}

/**
 * Lookup the prefix associated with the given namespace URI
 *
 * \param node       The node to start prefix search from
 * \param namespace  The namespace URI
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_node_lookup_prefix(struct dom_node *node,
		struct dom_string *namespace, struct dom_string **result)
{
	UNUSED(node);
	UNUSED(namespace);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Determine if the specified namespace is the default namespace
 *
 * \param node       The node to query
 * \param namespace  The namespace URI to test
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_node_is_default_namespace(struct dom_node *node,
		struct dom_string *namespace, bool *result)
{
	UNUSED(node);
	UNUSED(namespace);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Lookup the namespace URI associated with the given prefix
 *
 * \param node    The node to start namespace search from
 * \param prefix  The prefix to look for, or NULL to find default.
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_node_lookup_namespace(struct dom_node *node,
		struct dom_string *prefix, struct dom_string **result)
{
	UNUSED(node);
	UNUSED(prefix);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Determine if two DOM nodes are equal
 *
 * \param node    The node to compare
 * \param other   The node to compare against
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * Two nodes are equal iff:
 *   + They are of the same type
 *   + nodeName, localName, namespaceURI, prefix, nodeValue are equal
 *   + The node attributes are equal
 *   + The child nodes are equal
 *
 * Two DocumentType nodes are equal iff:
 *   + publicId, systemId, internalSubset are equal
 *   + The node entities are equal
 *   + The node notations are equal
 */
dom_exception dom_node_is_equal(struct dom_node *node,
		struct dom_node *other, bool *result)
{
	UNUSED(node);
	UNUSED(other);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve an object which implements the specialized APIs of the specified
 * feature and version.
 *
 * \param node     The node to query
 * \param feature  The requested feature
 * \param version  The version number of the feature
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_node_get_feature(struct dom_node *node,
		struct dom_string *feature, struct dom_string *version,
		void **result)
{
	UNUSED(node);
	UNUSED(feature);
	UNUSED(version);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Associate an object to a key on this node
 *
 * \param node     The node to insert object into
 * \param key      The key associated with the object
 * \param data     The object to associate with key, or NULL to remove
 * \param handler  User handler function, or NULL if none
 * \param result   Pointer to location to receive previously associated object
 * \return DOM_NO_ERR.
 */
dom_exception dom_node_set_user_data(struct dom_node *node,
		struct dom_string *key, void *data,
		dom_user_data_handler handler, void **result)
{
	struct dom_user_data *ud = NULL;
	void *prevdata = NULL;

	/* Search for user data */
	for (ud = node->user_data; ud != NULL; ud = ud->next) {
		if (dom_string_cmp(ud->key, key) == 0)
			break;
	};

	/* Remove it, if found and no new data */
	if (data == NULL && ud != NULL) {
		dom_string_unref(ud->key);

		if (ud->next != NULL)
			ud->next->prev = ud->prev;
		if (ud->prev != NULL)
			ud->prev->next = ud->next;
		else
			node->user_data = ud->next;

		*result = ud->data;

		dom_document_alloc(node->owner, ud, 0);

		return DOM_NO_ERR;
	}

	/* Otherwise, create a new user data object if one wasn't found */
	if (ud == NULL) {
		ud = dom_document_alloc(node->owner, NULL, 
				sizeof(struct dom_user_data));
		if (ud == NULL)
			return DOM_NO_MEM_ERR;

		dom_string_ref(key);
		ud->key = key;
		ud->data = NULL;
		ud->handler = NULL;

		/* Insert into list */
		ud->prev = NULL;
		ud->next = node->user_data;
		if (node->user_data)
			node->user_data->prev = ud;
		node->user_data = ud;
	}

	prevdata = ud->data;

	/* And associate data with it */
	ud->data = data;
	ud->handler = handler;

	*result = prevdata;

	return DOM_NO_ERR;
}

/**
 * Retrieves the object associated to a key on this node
 *
 * \param node    The node to retrieve object from
 * \param key     The key to search for
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_node_get_user_data(struct dom_node *node,
		struct dom_string *key, void **result)
{
	struct dom_user_data *ud = NULL;

	/* Search for user data */
	for (ud = node->user_data; ud != NULL; ud = ud->next) {
		if (dom_string_cmp(ud->key, key) == 0)
			break;
	};

	if (ud != NULL)
		*result = ud->data;
	else
		*result = NULL;

	return DOM_NO_ERR;
}

/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */

/**
 * Determine if a node is permitted as a child of another node
 *
 * \param parent  Prospective parent
 * \param child   Prospective child
 * \return true if ::child is permitted as a child of ::parent, false otherwise.
 */
bool _dom_node_permitted_child(const struct dom_node *parent, 
		const struct dom_node *child)
{
	bool valid;

	/* See DOM3Core $1.1.1 for details */

	switch (parent->type) {
	case DOM_ELEMENT_NODE:
	case DOM_ENTITY_REFERENCE_NODE:
	case DOM_ENTITY_NODE:
	case DOM_DOCUMENT_FRAGMENT_NODE:
		valid = (child->type == DOM_ELEMENT_NODE || 
			 child->type == DOM_TEXT_NODE || 
			 child->type == DOM_COMMENT_NODE || 
			 child->type == DOM_PROCESSING_INSTRUCTION_NODE || 
			 child->type == DOM_CDATA_SECTION_NODE || 
			 child->type == DOM_ENTITY_REFERENCE_NODE);
		break;

	case DOM_ATTRIBUTE_NODE:
		valid = (child->type == DOM_TEXT_NODE ||
			 child->type == DOM_ENTITY_REFERENCE_NODE);
		break;

	case DOM_TEXT_NODE:
	case DOM_CDATA_SECTION_NODE:
	case DOM_PROCESSING_INSTRUCTION_NODE:
	case DOM_COMMENT_NODE:
	case DOM_DOCUMENT_TYPE_NODE:
	case DOM_NOTATION_NODE:
		valid = false;
		break;

	case DOM_DOCUMENT_NODE:
		valid = (child->type == DOM_ELEMENT_NODE ||
			 child->type == DOM_PROCESSING_INSTRUCTION_NODE ||
			 child->type == DOM_COMMENT_NODE ||
			 child->type == DOM_DOCUMENT_TYPE_NODE);

		/* Ensure that the document doesn't already 
		 * have a root element */
		if (child->type == DOM_ELEMENT_NODE) {
			for (struct dom_node *n = parent->first_child; 
					n != NULL; n = n->next) {
				if (n->type == DOM_ELEMENT_NODE)
					valid = false;
			}
		}

		/* Ensure that the document doesn't already 
		 * have a document type */
		if (child->type == DOM_DOCUMENT_TYPE_NODE) {
			for (struct dom_node *n = parent->first_child;
					n != NULL; n = n->next) {
				if (n->type == DOM_DOCUMENT_TYPE_NODE)
					valid = false;
			}
		}

		break;
	}

	return valid;
}

/**
 * Determine if a node is read only
 *
 * \param node  The node to consider
 */
bool _dom_node_readonly(const struct dom_node *node)
{
	/* DocumentType and Notation nodes are read only */
	if (node->type == DOM_DOCUMENT_TYPE_NODE ||
			node->type == DOM_NOTATION_NODE)
		return true;

	/* Entity nodes and their descendants are read only */
	for (; node != NULL; node = node->parent) {
		if (node->type == DOM_ENTITY_NODE)
			return true;
	}

	/* EntityReference nodes and their descendants are read only */
	for (; node != NULL; node = node->parent) {
		if (node->type == DOM_ENTITY_REFERENCE_NODE)
			return true;
	}

	/* Otherwise, it's writable */
	return false;
}

/**
 * Attach a node to the tree
 *
 * \param node      The node to attach
 * \param parent    Node to attach ::node as child of
 * \param previous  Previous node in sibling list, or NULL if none
 * \param next      Next node in sibling list, or NULL if none
 */
inline void _dom_node_attach(struct dom_node *node, struct dom_node *parent, 
		struct dom_node *previous, struct dom_node *next)
{
	_dom_node_attach_range(node, node, parent, previous, next);
}

/**
 * Detach a node from the tree
 *
 * \param node  The node to detach
 */
inline void _dom_node_detach(struct dom_node *node)
{
	_dom_node_detach_range(node, node);
}

/**
 * Attach a range of nodes to the tree
 *
 * \param first     First node in the range
 * \param last      Last node in the range
 * \param parent    Node to attach range to
 * \param previous  Previous node in sibling list, or NULL if none
 * \param next      Next node in sibling list, or NULL if none
 *
 * The range is assumed to be a linked list of sibling nodes.
 */
inline void _dom_node_attach_range(struct dom_node *first, 
		struct dom_node *last,
		struct dom_node *parent, 
		struct dom_node *previous, 
		struct dom_node *next)
{
	first->previous = previous;
	last->next = next;

	if (previous != NULL)
		previous->next = first;
	else
		parent->first_child = first;

	if (next != NULL)
		next->previous = last;
	else
		parent->last_child = last;

	for (struct dom_node *n = first; n != last->next; n = n->next)
		n->parent = parent;
}

/**
 * Detach a range of nodes from the tree
 *
 * \param first  The first node in the range
 * \param last   The last node in the range
 *
 * The range is assumed to be a linked list of sibling nodes.
 */
inline void _dom_node_detach_range(struct dom_node *first, 
		struct dom_node *last)
{
	if (first->previous != NULL)
		first->previous->next = last->next;
	else
		first->parent->first_child = last->next;

	if (last->next != NULL)
		last->next->previous = first->previous;
	else
		last->parent->last_child = first->previous;

	for (struct dom_node *n = first; n != last->next; n = n->next)
		n->parent = NULL;

	first->previous = NULL;
	last->next = NULL;
}

/**
 * Replace a node in the tree
 *
 * \param old          Node to replace
 * \param replacement  Replacement node
 *
 * This is not implemented in terms of attach/detach in case 
 * we want to perform any special replacement-related behaviour 
 * at a later date.
 */
inline void _dom_node_replace(struct dom_node *old,
		struct dom_node *replacement)
{
	struct dom_node *first, *last;

	if (replacement->type == DOM_DOCUMENT_FRAGMENT_NODE) {
		first = replacement->first_child;
		last = replacement->last_child;

		replacement->first_child = replacement->last_child = NULL;
	} else {
		first = replacement;
		last = replacement;
	}

	first->previous = old->previous;
	last->next = old->next;

	if (old->previous != NULL)
		old->previous->next = first;
	else
		old->parent->first_child = first;

	if (old->next != NULL)
		old->next->previous = last;
	else
		old->parent->last_child = last;

	for (struct dom_node *n = first; n != last->next; n = n->next)
		n->parent = old->parent;

	old->previous = old->next = old->parent = NULL;
}

