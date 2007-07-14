/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/core/string.h>

#include "core/document.h"
#include "core/node.h"
#include "utils/utils.h"

/**
 * Create a DOM node
 *
 * \param doc    The document which owns the node
 * \param type   The node type required
 * \param name   The node name, or NULL
 * \param value  The node value, or NULL
 * \param node   Pointer to location to receive created node
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned node will be referenced, so there is no need for the caller
 * to explicitly reference it.
 */
dom_exception dom_node_create(struct dom_document *doc, dom_node_type type,
		struct dom_string *name, struct dom_string *value,
		struct dom_node **node)
{
	struct dom_node *n;

	n = dom_document_alloc(doc, NULL, sizeof(struct dom_node));
	if (n == NULL)
		return DOM_NO_MEM_ERR;

	if (name != NULL)
		dom_string_ref(name);
	n->name = name;

	if (value != NULL)
		dom_string_ref(value);
	n->value = value;

	n->type = type;

	n->parent = NULL;
	n->first_child = NULL;
	n->last_child = NULL;
	n->previous = NULL;
	n->next = NULL;
	n->attributes = NULL;

	dom_node_ref((struct dom_node *) doc);
	n->owner = doc;

	/** \todo Namespace handling */
	n->namespace = NULL;
	n->prefix = NULL;
	n->localname = NULL;

	n->user_data = NULL;

	n->refcnt = 1;

	*node = n;

	return DOM_NO_ERR;
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
 * If the reference count reaches zero, any memory claimed by the
 * node will be released
 */
void dom_node_unref(struct dom_node *node)
{
	if (--node->refcnt == 0) {
		/** \todo implement */
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
	UNUSED(node);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(node);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(node);
	UNUSED(value);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the type of a DOM node
 *
 * \param node    The node to retrieve the type of
 * \param result  Pointer to location to receive node type
 * \return DOM_NO_ERR.
 */
dom_exception dom_node_get_node_type(struct dom_node *node, dom_node_type *result)
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
dom_exception dom_node_get_parent(struct dom_node *node,
		struct dom_node **result)
{
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
 * \todo Work out reference counting semantics of dom_node_list
 */
dom_exception dom_node_get_children(struct dom_node *node,
		struct dom_node_list **result)
{
	UNUSED(node);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
dom_exception dom_node_get_previous(struct dom_node *node,
		struct dom_node **result)
{
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
dom_exception dom_node_get_next(struct dom_node *node,
		struct dom_node **result)
{
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
 * \todo Work out reference counting semantics of dom_named_node_map
 */
dom_exception dom_node_get_attributes(struct dom_node *node,
		struct dom_named_node_map **result)
{
	UNUSED(node);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
dom_exception dom_node_get_owner(struct dom_node *node,
		struct dom_document **result)
{
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
 *                                         ::node,
 *         DOM_NOT_SUPPORTED_ERR           if ::node is of type Document and
 *                                         ::new_child is of type
 *                                         DocumentType or Element.
 *
 * If ::new_child is a DocumentFragment, all of its children are inserted.
 * If ::new_child is already in the tree, it is first removed.
 *
 * ::new_child's reference count will be increased. The caller should unref
 * it (as they should already have held a reference on the node)
 */
dom_exception dom_node_insert_before(struct dom_node *node,
		struct dom_node *new_child, struct dom_node *ref_child,
		struct dom_node **result)
{
	UNUSED(node);
	UNUSED(new_child);
	UNUSED(ref_child);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
 * ::new_child's reference count will be increased. The caller should unref
 * it (as they should already have held a reference on the node)
 *
 * ::old_child's reference count remains unmodified (::node's reference is
 * transferred to the caller). The caller should unref ::old_child once it
 * is finished with it.
 */
dom_exception dom_node_replace_child(struct dom_node *node,
		struct dom_node *new_child, struct dom_node *old_child,
		struct dom_node **result)
{
	UNUSED(node);
	UNUSED(new_child);
	UNUSED(old_child);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
 * ::old_child's reference count remains unmodified (::node's reference is
 * transferred to the caller). The caller should unref ::old_child once it
 * is finished with it.
 */
dom_exception dom_node_remove_child(struct dom_node *node,
		struct dom_node *old_child,
		struct dom_node **result)
{
	UNUSED(node);
	UNUSED(old_child);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
 *                                         ::new_child's parent is readonly,
 *         DOM_NOT_SUPPORTED_ERR           if ::node is of type Document and
 *                                         ::new_child is of type
 *                                         DocumentType or Element.
 *
 * If ::new_child is a DocumentFragment, all of its children are inserted.
 * If ::new_child is already in the tree, it is first removed.
 *
 * ::new_child's reference count will be increased. The caller should unref
 * it (as they should already have held a reference on the node)
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
dom_exception dom_node_has_children(struct dom_node *node, bool *result)
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
dom_exception dom_node_clone(struct dom_node *node, bool deep,
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
	UNUSED(node);
	UNUSED(key);
	UNUSED(data);
	UNUSED(handler);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(node);
	UNUSED(key);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}
