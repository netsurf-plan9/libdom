/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dom/dom.h>
#include <dom/core/attr.h>
#include <dom/core/element.h>
#include <dom/core/node.h>
#include <dom/core/string.h>
#include <dom/core/document.h>
#include <dom/events/events.h>

#include "core/attr.h"
#include "core/document.h"
#include "core/element.h"
#include "core/node.h"
#include "core/namednodemap.h"
#include "utils/validate.h"
#include "utils/namespace.h"
#include "utils/utils.h"
#include "utils/hashtable.h"
#include "events/mutation_event.h"

/* The three numbers are just random ones, maybe we should change it after some
 * more consideration */
#define CHAINS_ATTRIBUTES 31
#define CHAINS_NAMESPACE  7
#define CHAINS_NS_ATTRIBUTES 31

struct dom_element_vtable _dom_element_vtable = {
	{
		{
			DOM_NODE_EVENT_TARGET_VTABLE
		},
		DOM_NODE_VTABLE_ELEMENT
	},
	DOM_ELEMENT_VTABLE
};

static struct dom_element_protected_vtable element_protect_vtable = {
	{
		DOM_NODE_PROTECT_VTABLE_ELEMENT
	},
	DOM_ELEMENT_PROTECT_VTABLE
};

static dom_exception _dom_element_get_attr(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name, 
		dom_string **value);
static dom_exception _dom_element_set_attr(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name, 
		dom_string *value);
static dom_exception _dom_element_remove_attr(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name);
static dom_exception _dom_element_get_attr_node(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name, 
		struct dom_attr **result);
static dom_exception _dom_element_set_attr_node(struct dom_element *element,
		struct dom_hash_table *hs, struct dom_attr *attr, 
		struct dom_attr **result);
static dom_exception _dom_element_remove_attr_node(struct dom_element *element,
		struct dom_hash_table *hs, struct dom_attr *attr, 
		struct dom_attr **result);
static dom_exception _dom_element_has_attr(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name,
		bool *result);
static dom_exception _dom_element_set_id_attr(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name, bool is_id);


/* The operation set for namednodemap */
static dom_exception attributes_get_length(void *priv,
		unsigned long *length);
static dom_exception attributes_get_named_item(void *priv,
		dom_string *name, struct dom_node **node);
static dom_exception attributes_set_named_item(void *priv,
		struct dom_node *arg, struct dom_node **node);
static dom_exception attributes_remove_named_item(
		void *priv, dom_string *name,
		struct dom_node **node);
static dom_exception attributes_item(void *priv,
		unsigned long index, struct dom_node **node);
static dom_exception attributes_get_named_item_ns(
		void *priv, dom_string *namespace,
		dom_string *localname, struct dom_node **node);
static dom_exception attributes_set_named_item_ns(
		void *priv, struct dom_node *arg,
		struct dom_node **node);
static dom_exception attributes_remove_named_item_ns(
		void *priv, dom_string *namespace,
		dom_string *localname, struct dom_node **node);
static void attributes_destroy(void *priv);
static bool attributes_equal(void *p1, void *p2);

static struct nnm_operation attributes_opt = {
	attributes_get_length,
	attributes_get_named_item,
	attributes_set_named_item,
	attributes_remove_named_item,
	attributes_item,
	attributes_get_named_item_ns,
	attributes_set_named_item_ns,
	attributes_remove_named_item_ns,
	attributes_destroy,
	attributes_equal
};

static uint32_t attributes_hash(void *key, void *pw);
static void *attributes_clone_key(void *key, void *pw);
static void attributes_destroy_key(void *key, void *pw);
static void *attributes_clone_value(void *value, void *pw);
static void attributes_destroy_value(void *value, void *pw);
static void *attributes_ns_clone_value(void *value, void *pw);
static void attributes_ns_destroy_value(void *value, void *pw);
static bool attributes_key_isequal(void *key1, void *key2, void *pw);

static const struct dom_hash_vtable attributes_vtable = {
	attributes_hash,
	attributes_clone_key,
	attributes_destroy_key,
	attributes_clone_value,
	attributes_destroy_value,
	attributes_key_isequal
};

static const struct dom_hash_vtable attributes_ns_vtable = {
	attributes_hash,
	attributes_clone_key,
	attributes_destroy_key,
	attributes_ns_clone_value,
	attributes_ns_destroy_value,
	attributes_key_isequal
};

/*----------------------------------------------------------------------*/
/* Constructors and Destructors */

/**
 * Create an element node
 *
 * \param doc        The owning document
 * \param name       The (local) name of the node to create
 * \param namespace  The namespace URI of the element, or NULL
 * \param prefix     The namespace prefix of the element, or NULL
 * \param result     Pointer to location to receive created element
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::name is invalid,
 *         DOM_NO_MEM_ERR            on memory exhaustion.
 *
 * ::doc, ::name, ::namespace and ::prefix will have their 
 * reference counts increased.
 *
 * The returned element will already be referenced.
 */
dom_exception _dom_element_create(struct dom_document *doc,
		dom_string *name, dom_string *namespace,
		dom_string *prefix, struct dom_element **result)
{
	/* Allocate the element */
	*result = malloc(sizeof(struct dom_element));
	if (*result == NULL)
		return DOM_NO_MEM_ERR;

	/* Initialise the vtables */
	(*result)->base.base.vtable = &_dom_element_vtable;
	(*result)->base.vtable = &element_protect_vtable;

	return _dom_element_initialise(doc, *result, name, namespace, prefix);
}

/**
 * Initialise an element node
 *
 * \param doc        The owning document
 * \param el	     The element
 * \param name       The (local) name of the node to create
 * \param namespace  The namespace URI of the element, or NULL
 * \param prefix     The namespace prefix of the element, or NULL
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::name is invalid,
 *         DOM_NO_MEM_ERR            on memory exhaustion.
 *
 * The caller should make sure that ::name is a valid NCName.
 *
 * ::doc, ::name, ::namespace and ::prefix will have their 
 * reference counts increased.
 *
 * The returned element will already be referenced.
 */
dom_exception _dom_element_initialise(struct dom_document *doc,
		struct dom_element *el, dom_string *name, 
		dom_string *namespace, dom_string *prefix)
{
	dom_exception err;

	assert(doc != NULL);

	el->attributes = _dom_hash_create(CHAINS_ATTRIBUTES, 
			&attributes_vtable, NULL);
	if (el->attributes == NULL) {
		free(el);
		return DOM_NO_MEM_ERR;;
	}

	el->ns_attributes = _dom_hash_create(CHAINS_NAMESPACE, 
			&attributes_ns_vtable, NULL);
	if (el->ns_attributes == NULL) {
		free(el->attributes);
		free(el);
		return DOM_NO_MEM_ERR;
	}

	/* Initialise the base class */
	err = _dom_node_initialise(&el->base, doc, DOM_ELEMENT_NODE,
			name, NULL, namespace, prefix);
	if (err != DOM_NO_ERR) {
		free(el->ns_attributes);
		free(el->attributes);
		free(el);
		return err;
	}

	/* Perform our type-specific initialisation */
	el->id_ns = NULL;
	el->id_name = NULL;
	el->schema_type_info = NULL;

	return DOM_NO_ERR;
}

/**
 * Finalise a dom_element
 *
 * \param ele  The element
 */
void _dom_element_finalise(struct dom_element *ele)
{
	/* Destroy attributes attached to this node */
	if (ele->attributes != NULL) {
		_dom_hash_destroy(ele->attributes);
		ele->attributes = NULL;
	}

	if (ele->ns_attributes != NULL) {
		_dom_hash_destroy(ele->ns_attributes);
		ele->ns_attributes = NULL;
	}

	if (ele->schema_type_info != NULL) {
		/** \todo destroy schema type info */
	}

	/* Finalise base class */
	_dom_node_finalise(&ele->base);
}

/**
 * Destroy an element
 *
 * \param element  The element to destroy
 *
 * The contents of ::element will be destroyed and ::element will be freed.
 */
void _dom_element_destroy(struct dom_element *element)
{
	_dom_element_finalise(element);

	/* Free the element */
	free(element);
}

/*----------------------------------------------------------------------*/

/* The public virtual functions */

/**
 * Retrieve an element's tag name
 *
 * \param element  The element to retrieve the name from
 * \param name     Pointer to location to receive name
 * \return DOM_NO_ERR      on success,
 *         DOM_NO_MEM_ERR  on memory exhaustion.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_element_get_tag_name(struct dom_element *element,
		dom_string **name)
{
	/* This is the same as nodeName */
	return dom_node_get_node_name((struct dom_node *) element, name);
}

/**
 * Retrieve an attribute from an element by name
 *
 * \param element  The element to retrieve attribute from
 * \param name     The attribute's name
 * \param value    Pointer to location to receive attribute's value
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_element_get_attribute(struct dom_element *element,
		dom_string *name, dom_string **value)
{
	return _dom_element_get_attr(element, element->attributes, name, value);
}

/**
 * Set an attribute on an element by name
 *
 * \param element  The element to set attribute on
 * \param name     The attribute's name
 * \param value    The attribute's value
 * \return DOM_NO_ERR                      on success,
 *         DOM_INVALID_CHARACTER_ERR       if ::name is invalid,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly.
 */
dom_exception _dom_element_set_attribute(struct dom_element *element,
		dom_string *name, dom_string *value)
{
	return _dom_element_set_attr(element, element->attributes, name, value);
}

/**
 * Remove an attribute from an element by name
 *
 * \param element  The element to remove attribute from
 * \param name     The name of the attribute to remove
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly.
 */
dom_exception _dom_element_remove_attribute(struct dom_element *element,
		dom_string *name)
{
	return _dom_element_remove_attr(element, element->attributes, name);
}

/**
 * Retrieve an attribute node from an element by name
 *
 * \param element  The element to retrieve attribute node from
 * \param name     The attribute's name
 * \param result   Pointer to location to receive attribute node
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_get_attribute_node(struct dom_element *element, 
		dom_string *name, struct dom_attr **result)
{
	return _dom_element_get_attr_node(element, element->attributes, name, 
			result);
}

/**
 * Set an attribute node on an element, replacing existing node, if present
 *
 * \param element  The element to add a node to
 * \param attr     The attribute node to add
 * \param result   Pointer to location to receive previous node
 * \return DOM_NO_ERR                      on success,
 *         DOM_WRONG_DOCUMENT_ERR          if ::attr does not belong to the
 *                                         same document as ::element,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_INUSE_ATTRIBUTE_ERR         if ::attr is already an attribute
 *                                         of another Element node.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_set_attribute_node(struct dom_element *element,
		struct dom_attr *attr, struct dom_attr **result)
{
	return _dom_element_set_attr_node(element, element->attributes, attr, 
			result);
}

/**
 * Remove an attribute node from an element
 *
 * \param element  The element to remove attribute node from
 * \param attr     The attribute node to remove
 * \param result   Pointer to location to receive attribute node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_FOUND_ERR               if ::attr is not an attribute of
 *                                         ::element.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_remove_attribute_node(struct dom_element *element,
		struct dom_attr *attr, struct dom_attr **result)
{
	return _dom_element_remove_attr_node(element, element->attributes,
			attr, result);
}

/**
 * Retrieve a list of descendant elements of an element which match a given
 * tag name
 *
 * \param element  The root of the subtree to search
 * \param name     The tag name to match (or "*" for all tags)
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned nodelist will have its reference count increased. It is
 * the responsibility of the caller to unref the nodelist once it has
 * finished with it.
 */
dom_exception _dom_element_get_elements_by_tag_name(
		struct dom_element *element, dom_string *name,
		struct dom_nodelist **result)
{
	dom_exception err;
	dom_node_internal *base = (dom_node_internal *) element;
	
	assert(base->owner != NULL);

	err = _dom_document_get_nodelist(base->owner, DOM_NODELIST_BY_NAME,
			(struct dom_node_internal *) element, name, NULL, 
			NULL, result);

	return err;
}

/**
 * Retrieve an attribute from an element by namespace/localname
 *
 * \param element    The element to retrieve attribute from
 * \param namespace  The attribute's namespace URI, or NULL
 * \param localname  The attribute's local name
 * \param value      Pointer to location to receive attribute's value
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support
 *                               the feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_element_get_attribute_ns(struct dom_element *element,
		dom_string *namespace, dom_string *localname,
		dom_string **value)
{
	struct dom_hash_table *attrs;

	if (namespace == NULL)
		return _dom_element_get_attribute(element, localname, value);

	attrs = _dom_hash_get(element->ns_attributes, namespace);
	/* The element has no such namespace */
	if (attrs == NULL) {
		*value = NULL;
		return DOM_NO_ERR;
	}

	return _dom_element_get_attr(element, attrs, localname, value);
}

/**
 * Set an attribute on an element by namespace/qualified name
 *
 * \param element    The element to set attribute on
 * \param namespace  The attribute's namespace URI
 * \param qname      The attribute's qualified name
 * \param value      The attribute's value
 * \return DOM_NO_ERR                      on success,
 *         DOM_INVALID_CHARACTER_ERR       if ::qname is invalid,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NAMESPACE_ERR               if ::qname is malformed, or
 *                                         ::qname has a prefix and
 *                                         ::namespace is null, or ::qname
 *                                         has a prefix "xml" and
 *                                         ::namespace is not
 *                                         "http://www.w3.org/XML/1998/namespace",
 *                                         or ::qname has a prefix "xmlns"
 *                                         and ::namespace is not
 *                                         "http://www.w3.org/2000/xmlns",
 *                                         or ::namespace is
 *                                         "http://www.w3.org/2000/xmlns"
 *                                         and ::qname is not prefixed
 *                                         "xmlns",
 *         DOM_NOT_SUPPORTED_ERR           if the implementation does not
 *                                         support the feature "XML" and the
 *                                         language exposed through the
 *                                         Document does not support
 *                                         Namespaces.
 */
dom_exception _dom_element_set_attribute_ns(struct dom_element *element,
		dom_string *namespace, dom_string *qname,
		dom_string *value)
{
	dom_exception err;
	struct dom_hash_table *attrs;
	bool added;

	if (_dom_validate_name(qname) == false)
		return DOM_INVALID_CHARACTER_ERR;

	err = _dom_namespace_validate_qname(qname, namespace);
	if (err != DOM_NO_ERR)
		return DOM_NAMESPACE_ERR;

	dom_string *localname;
	dom_string *prefix;
	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR)
		return err;

	/* If there is no namespace, redirect to set_attribute */
	if (namespace == NULL) {
		if (prefix != NULL)
			return DOM_NAMESPACE_ERR;
		err = _dom_element_set_attribute(element, localname, value);
		dom_string_unref(localname);
		return err;
	}

	attrs = _dom_hash_get(element->ns_attributes, namespace);
	/* The element has no such namespace */
	if (attrs == NULL) {
		attrs = _dom_hash_create(CHAINS_NS_ATTRIBUTES,
				&attributes_vtable, NULL);
		if (attrs == NULL)
			return DOM_NO_MEM_ERR;

		added = _dom_hash_add(element->ns_attributes, namespace, attrs,
				false);
		if (added == false)
			return DOM_NO_MEM_ERR;
		dom_string_ref(namespace);
	}

	return _dom_element_set_attr(element, attrs, localname, value);
}

/**
 * Remove an attribute from an element by namespace/localname
 *
 * \param element    The element to remove attribute from
 * \param namespace  The attribute's namespace URI
 * \param localname  The attribute's local name
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_SUPPORTED_ERR           if the implementation does not
 *                                         support the feature "XML" and the
 *                                         language exposed through the
 *                                         Document does not support
 *                                         Namespaces.
 */
dom_exception _dom_element_remove_attribute_ns(struct dom_element *element,
		dom_string *namespace, dom_string *localname)
{
	struct dom_hash_table *attrs;

	if (namespace != NULL)
		return _dom_element_remove_attribute(element, localname);

	attrs = _dom_hash_get(element->ns_attributes, namespace);
	/* The element has no such namespace */
	if (attrs == NULL) {
		return DOM_NO_ERR;
	}

	return _dom_element_remove_attr(element, attrs, localname);
}

/**
 * Retrieve an attribute node from an element by namespace/localname
 *
 * \param element    The element to retrieve attribute from
 * \param namespace  The attribute's namespace URI
 * \param localname  The attribute's local name
 * \param result     Pointer to location to receive attribute node
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support
 *                               the feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_get_attribute_node_ns(struct dom_element *element,
		dom_string *namespace, dom_string *localname,
		struct dom_attr **result)
{
	struct dom_hash_table *attrs;

	if (namespace == NULL) {
		return _dom_element_get_attribute_node(element, localname,
				result);
	}

	attrs = _dom_hash_get(element->ns_attributes, namespace);
	/* The element has no such namespace */
	if (attrs == NULL) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	return _dom_element_get_attr_node(element, attrs, localname, result);
}

/**
 * Set an attribute node on an element, replacing existing node, if present
 *
 * \param element  The element to add a node to
 * \param attr     The attribute node to add
 * \param result   Pointer to location to recieve previous node
 * \return DOM_NO_ERR                      on success,
 *         DOM_WRONG_DOCUMENT_ERR          if ::attr does not belong to the
 *                                         same document as ::element,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_INUSE_ATTRIBUTE_ERR         if ::attr is already an attribute
 *                                         of another Element node.
 *         DOM_NOT_SUPPORTED_ERR           if the implementation does not
 *                                         support the feature "XML" and the
 *                                         language exposed through the
 *                                         Document does not support
 *                                         Namespaces.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_set_attribute_node_ns(struct dom_element *element,
		struct dom_attr *attr, struct dom_attr **result)
{
	dom_exception err;
	struct dom_hash_table *attrs;
	bool added;
	dom_string *namespace;

	err = dom_node_get_namespace(attr, (void *) &namespace);
	if (err != DOM_NO_ERR)
		return err;

	if (namespace == NULL)
		return _dom_element_set_attribute_node(element, attr, result);

	attrs = _dom_hash_get(element->ns_attributes, namespace);
	/* The element has no such namespace */
	if (attrs == NULL) {
		attrs = _dom_hash_create(CHAINS_NS_ATTRIBUTES,
				&attributes_vtable, NULL);
		if (attrs == NULL)
			return DOM_NO_MEM_ERR;

		added = _dom_hash_add(element->ns_attributes, namespace, attrs,
				false);
		if (added == false)
			return DOM_NO_MEM_ERR;
		dom_string_ref(namespace);
	}

	dom_string *localname;
	err = dom_node_get_local_name(attr, (void *) &localname);
	if (err != DOM_NO_ERR)
		return err;

	return _dom_element_set_attr_node(element, attrs, attr, result);
}

/**
 * Retrieve a list of descendant elements of an element which match a given
 * namespace/localname pair.
 *
 * \param element  The root of the subtree to search
 * \param namespace  The namespace URI to match (or "*" for all)
 * \param localname  The local name to match (or "*" for all)
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support
 *                               the feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 *
 * The returned nodelist will have its reference count increased. It is
 * the responsibility of the caller to unref the nodelist once it has
 * finished with it.
 */
dom_exception _dom_element_get_elements_by_tag_name_ns(
		struct dom_element *element, dom_string *namespace,
		dom_string *localname, struct dom_nodelist **result)
{
	dom_exception err;

	/** \todo ensure XML feature is supported */

	err = _dom_document_get_nodelist(element->base.owner,
			DOM_NODELIST_BY_NAMESPACE,
			(struct dom_node_internal *) element, NULL, 
			namespace, localname,
			result);

	return err;
}

/**
 * Determine if an element possesses and attribute with the given name
 *
 * \param element  The element to query
 * \param name     The attribute name to look for
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_element_has_attribute(struct dom_element *element,
		dom_string *name, bool *result)
{
	return _dom_element_has_attr(element, element->attributes, name,
			result);
}

/**
 * Determine if an element possesses and attribute with the given
 * namespace/localname pair.
 *
 * \param element    The element to query
 * \param namespace  The attribute namespace URI to look for
 * \param localname  The attribute local name to look for
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the implementation does not support
 *                               the feature "XML" and the language exposed
 *                               through the Document does not support
 *                               Namespaces.
 */
dom_exception _dom_element_has_attribute_ns(struct dom_element *element,
		dom_string *namespace, dom_string *localname,
		bool *result)
{
	struct dom_hash_table *attrs;

	if (namespace == NULL)
		return _dom_element_has_attribute(element, localname, result);

	attrs = _dom_hash_get(element->ns_attributes, namespace);
	/* The element has no such namespace */
	if (attrs == NULL) {
		*result = false;
		return DOM_NO_ERR;
	}

	return _dom_element_has_attr(element, attrs, localname, result);
}

/**
 * Retrieve the type information associated with an element
 *
 * \param element  The element to retrieve type information from
 * \param result   Pointer to location to receive type information
 * \return DOM_NO_ERR.
 *
 * The returned typeinfo will have its reference count increased. It is
 * the responsibility of the caller to unref the typeinfo once it has
 * finished with it.
 */
dom_exception _dom_element_get_schema_type_info(struct dom_element *element,
		struct dom_type_info **result)
{
	UNUSED(element);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * (Un)declare an attribute as being an element's ID by name
 *
 * \param element  The element containing the attribute
 * \param name     The attribute's name
 * \param is_id    Whether the attribute is an ID
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_FOUND_ERR               if the specified node is not an
 *                                         attribute of ::element.
 *
 * @note: The DOM spec does not say: how to deal with when there are two or 
 * more isId attribute nodes. Here, the implementation just maintain only
 * one such attribute node.
 */
dom_exception _dom_element_set_id_attribute(struct dom_element *element,
		dom_string *name, bool is_id)
{
	return _dom_element_set_id_attr(element, element->attributes, name, 
			is_id);
}

/**
 * (Un)declare an attribute as being an element's ID by namespace/localname
 *
 * \param element    The element containing the attribute
 * \param namespace  The attribute's namespace URI
 * \param localname  The attribute's local name
 * \param is_id      Whether the attribute is an ID
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_FOUND_ERR               if the specified node is not an
 *                                         attribute of ::element.
 */
dom_exception _dom_element_set_id_attribute_ns(struct dom_element *element,
		dom_string *namespace, dom_string *localname,
		bool is_id)
{
	struct dom_hash_table *hs;
	dom_exception err;

	if (namespace == NULL)
		return _dom_element_set_id_attribute(element, localname, is_id);

	hs = _dom_hash_get(element->ns_attributes, namespace);
	assert(hs != NULL);

	err = _dom_element_set_id_attr(element, hs, localname, is_id);
	if (err != DOM_NO_ERR)
		return err;
	
	element->id_ns = dom_string_ref(namespace);

	return DOM_NO_ERR;
}

/**
 * (Un)declare an attribute node as being an element's ID
 *
 * \param element  The element containing the attribute
 * \param id_attr  The attribute node
 * \param is_id    Whether the attribute is an ID
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_FOUND_ERR               if the specified node is not an
 *                                         attribute of ::element.
 */
dom_exception _dom_element_set_id_attribute_node(struct dom_element *element,
		struct dom_attr *id_attr, bool is_id)
{
	struct dom_hash_table *hs;
	dom_exception err;
	dom_string *namespace;
	dom_string *localname;

	err = dom_node_get_namespace(id_attr, &namespace);
	if (err != DOM_NO_ERR)
		return err;
	err = dom_node_get_local_name(id_attr, &localname);
	if (err != DOM_NO_ERR)
		return err;

	hs = _dom_hash_get(element->ns_attributes, namespace);
	assert(hs != NULL);

	err = _dom_element_set_id_attr(element, hs, localname, is_id);
	if (err != DOM_NO_ERR)
		return err;
	
	element->id_ns = namespace;

	return DOM_NO_ERR;

}

/**
 * Obtain a pre-parsed array of class names for an element
 *
 * \param element    Element containing classes
 * \param classes    Pointer to location to receive allocated array
 * \param n_classes  Pointer to location to receive number of classes
 * \return DOM_NO_ERR on success,
 *         DOM_NO_MEM_ERR on memory exhaustion
 */
dom_exception _dom_element_get_classes(struct dom_element *element,
		lwc_string ***classes, uint32_t *n_classes)
{
	UNUSED(element);
	UNUSED(classes);
	UNUSED(n_classes);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Determine if an element has an associated class
 *
 * \param element  Element to consider
 * \param name     Class name to look for
 * \param match    Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_element_has_class(struct dom_element *element,
		lwc_string *name, bool *match)
{
	UNUSED(element);
	UNUSED(name);
	UNUSED(match);

	return DOM_NOT_SUPPORTED_ERR;
}

/*------------- The overload virtual functions ------------------------*/

/* Overload function of Node, please refer src/core/node.c for detail */
dom_exception _dom_element_get_attributes(dom_node_internal *node,
		struct dom_namednodemap **result)
{
	dom_exception err;
	dom_document *doc;

	doc = dom_node_get_owner(node);
	assert(doc != NULL);

	err = _dom_namednodemap_create(doc, node, &attributes_opt, result);
	if (err != DOM_NO_ERR)
		return err;
	
	dom_node_ref(node);
	
	return DOM_NO_ERR;
}

/* Overload function of Node, please refer src/core/node.c for detail */
dom_exception _dom_element_has_attributes(dom_node_internal *node, bool *result)
{
	UNUSED(node);
	*result = true;

	return DOM_NO_ERR;
}

/* For the following namespace related algorithm take a look at:
 * http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/namespaces-algorithms.html
 */

/**
 * Look up the prefix which matches the namespace.
 *
 * \param node       The current Node in which we search for
 * \param namespace  The namespace for which we search a prefix
 * \param result     The returned prefix
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_lookup_prefix(dom_node_internal *node,
		dom_string *namespace, dom_string **result)
{
	struct dom_element *owner;
	dom_exception err;

	err = dom_attr_get_owner_element(node, &owner);
	if (err != DOM_NO_ERR)
		return err;
	
	if (owner == NULL) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	return dom_node_lookup_prefix(owner, namespace, result);
}

/**
 * Test whether certain namespace is the default namespace of some node.
 *
 * \param node       The Node to test
 * \param namespace  The namespace to test
 * \param result     true is the namespace is default namespace
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_is_default_namespace(dom_node_internal *node,
		dom_string *namespace, bool *result)
{
	struct dom_element *ele = (struct dom_element *) node;
	dom_string *value;
	dom_exception err;

	if (node->prefix == NULL) {
		*result = dom_string_isequal(node->namespace, namespace);
		return DOM_NO_ERR;
	}

	bool has;
	dom_string *xmlns = _dom_namespace_get_xmlns_prefix();
	err = dom_element_has_attribute(ele, xmlns, &has);
	if (err != DOM_NO_ERR)
		return err;
	
	if (has == true) {
		err = dom_element_get_attribute(ele, xmlns, &value);
		if (err != DOM_NO_ERR)
			return err;

		*result = dom_string_isequal(value, namespace);

		dom_string_unref(value);

		return DOM_NO_ERR;
	}

	return dom_node_is_default_namespace(node->parent, namespace, result);
}

/**
 * Look up the namespace with certain prefix.
 *
 * \param node    The current node in which we search for the prefix
 * \param prefix  The prefix to search
 * \param result  The result namespace if found
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_lookup_namespace(dom_node_internal *node,
		dom_string *prefix, dom_string **result)
{
	dom_exception err;

	if (node->namespace != NULL && 
			dom_string_isequal(node->prefix, prefix)) {
		*result = dom_string_ref(node->namespace);
		return DOM_NO_ERR;
	}
	
	bool has;
	dom_string *xmlns = _dom_namespace_get_xmlns_prefix();
	err = dom_element_has_attribute_ns(node, xmlns, prefix, &has);
	if (err != DOM_NO_ERR)
		return err;
	
	if (has == true)
		return dom_element_get_attribute_ns(node,
				dom_namespaces[DOM_NAMESPACE_XMLNS], prefix,
				result);

	err = dom_element_has_attribute(node, xmlns, &has);
	if (err != DOM_NO_ERR)
		return err;
	
	if (has == true) {
		return dom_element_get_attribute(node, xmlns, result);
	}

	return dom_node_lookup_namespace(node->parent, prefix, result);
}


/*----------------------------------------------------------------------*/
/* The protected virtual functions */

/**
 * The virtual function to parse some dom attribute
 *
 * \param ele     The element object
 * \param name    The name of the attribute
 * \param value   The new value of the attribute
 * \param parsed  The parsed value of the attribute
 * \return DOM_NO_ERR on success.
 *
 * @note: This virtual method is provided to serve as a template method. 
 * When any attribute is set or added, the attribute's value should be
 * checked to make sure that it is a valid one. And the child class of 
 * dom_element may to do some special stuff on the attribute is set. Take
 * some integer attribute as example:
 *
 * 1. The client call dom_element_set_attribute("size", "10.1"), but the
 *    size attribute may only accept an integer, and only the specific
 *    dom_element know this. And the dom_attr_set_value method, which is
 *    called by dom_element_set_attribute should call the this virtual
 *    template method.
 * 2. The overload virtual function of following one will truncate the
 *    "10.1" to "10" to make sure it is a integer. And of course, the 
 *    overload method may also save the integer as a 'int' C type for 
 *    later easy accessing by any client.
 */
dom_exception _dom_element_parse_attribute(dom_element *ele,
		dom_string *name, dom_string *value,
		dom_string **parsed)
{
	UNUSED(ele);
	UNUSED(name);

	dom_string_ref(value);
	*parsed = value;

	return DOM_NO_ERR;
}

/* The destroy virtual function of dom_element */
void __dom_element_destroy(struct dom_node_internal *node)
{
	_dom_element_destroy((struct dom_element *) node);
}

/* TODO: How to deal with default attribue:
 *
 *  Ask a language binding for default attributes.	
 *
 *	So, when we copy a element we copy all its attributes because they
 *	are all specified. For the methods like importNode and adoptNode, 
 *	this will make _dom_element_copy can be used in them.
 */
dom_exception _dom_element_copy(dom_node_internal *old, 
		dom_node_internal **copy)
{
	dom_element *olde = (dom_element *) old;
	dom_element *e;
	struct dom_hash_table *ht;
	dom_exception err;
	
	e = malloc(sizeof(dom_element));
	if (e == NULL)
		return DOM_NO_MEM_ERR;

	err = dom_node_copy_internal(old, e);
	if (err != DOM_NO_ERR) {
		free(e);
		return err;
	}
	
	/* Copy the hash tables */
	ht = _dom_hash_clone(olde->attributes);
	if (ht == NULL)
		return DOM_NO_MEM_ERR;
	e->attributes = ht;

	ht = _dom_hash_clone(olde->ns_attributes);
	if (ht == NULL)
		return DOM_NO_MEM_ERR;
	e->ns_attributes = ht;

	e->id_ns = NULL;
	e->id_name = NULL;

	/* TODO: deal with dom_type_info, it get no definition ! */

	*copy = (dom_node_internal *) e;

	return DOM_NO_ERR;
}



/*--------------------------------------------------------------------------*/

/* Helper functions */

/**
 * The internal helper function for getAttribute/getAttributeNS.
 *
 * \param element  The element
 * \param hs       The hash table contains the attributes
 * \param name     The name of the attribute
 * \param value    The value of the attribute
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_get_attr(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name, 
		dom_string **value)
{
	void *a;
	dom_exception err = DOM_NO_ERR;

	UNUSED(element);

	a = _dom_hash_get(hs, name);

	/* Fill in value */
	if (a == NULL) {
		*value = NULL;
	} else {
		err = dom_attr_get_value(((struct dom_attr *) a), value);
	}

	return err;
}

/**
 * The internal helper function for setAttribute and setAttributeNS.
 *
 * \param element  The element
 * \param hs       The attributes' hash table
 * \param name     The name of the new attribute
 * \param value    The value of the new attribute
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_set_attr(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name, 
		dom_string *value)
{
	void *a;
	bool added;
	dom_node_internal *e = (dom_node_internal *) element;

	if (_dom_validate_name(name) == false)
		return DOM_INVALID_CHARACTER_ERR;

	/* Ensure element can be written */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	a = _dom_hash_get(hs, name);

	if (a != NULL) {
		/* Found an existing attribute, so replace its value */
		dom_exception err;

		/* Dispatch a DOMAttrModified event */
		dom_string *old = NULL;
		struct dom_document *doc = dom_node_get_owner(element);
		bool success = true;
		err = dom_attr_get_value(a, &old);
		/* TODO: We did not support some node type such as entity
		 * reference, in that case, we should ignore the error to
		 * make sure the event model work as excepted. */
		if (err != DOM_NO_ERR && err != DOM_NOT_SUPPORTED_ERR)
			return err;
		err = _dom_dispatch_attr_modified_event(doc, e, old, value, a,
				name, DOM_MUTATION_MODIFICATION, &success);
		dom_string_unref(old);
		if (err != DOM_NO_ERR)
			return err;

		success = true;
		err = _dom_dispatch_subtree_modified_event(doc,
				(dom_event_target *) e, &success);
		if (err != DOM_NO_ERR)
			return err;

		err = dom_attr_set_value((struct dom_attr *) a, value);
		if (err != DOM_NO_ERR)
			return err;
	} else {
		/* No existing attribute, so create one */
		dom_exception err;
		struct dom_attr *attr;

		err = _dom_attr_create(e->owner, name, NULL, NULL, true, &attr);
		if (err != DOM_NO_ERR)
			return err;

		/* Set its parent, so that value parsing works */
		dom_node_set_parent(attr, element);

		/* Set its value */
		err = dom_attr_set_value(attr, value);
		if (err != DOM_NO_ERR) {
			dom_node_set_parent(attr, NULL);
			dom_node_unref(attr);
			return err;
		}

		/* Dispatch a DOMAttrModified event */
		struct dom_document *doc = dom_node_get_owner(element);
		bool success = true;
		err = _dom_dispatch_attr_modified_event(doc, e, NULL, value,
				(dom_event_target *) attr, name, 
				DOM_MUTATION_ADDITION, &success);
		if (err != DOM_NO_ERR) {
			dom_node_set_parent(attr, NULL);
			dom_node_unref(attr);
			return err;
		}

		err = dom_node_dispatch_node_change_event(doc,
				attr, element, DOM_MUTATION_ADDITION, &success);
		if (err != DOM_NO_ERR) {
			dom_node_set_parent(attr, NULL);
			dom_node_unref(attr);
			return err;
		}

		added = _dom_hash_add(hs, name, attr, false);
		if (added == false) {
			/* If we failed at this step, there must be no memory */
			dom_node_set_parent(attr, NULL);
			dom_node_unref(attr);
			return DOM_NO_MEM_ERR;
		}
		dom_string_ref(name);

		dom_node_unref(attr);
		dom_node_remove_pending(attr);

		success = true;
		err = _dom_dispatch_subtree_modified_event(doc,
				(dom_event_target *) element, &success);
		if (err != DOM_NO_ERR)
			return err;
	}

	return DOM_NO_ERR;
}

/**
 * Remove an attribute from an element by name
 *
 * \param element  The element to remove attribute from
 * \param name     The name of the attribute to remove
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly.
 */
dom_exception _dom_element_remove_attr(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name)
{
	void *a;
	dom_exception err;
	dom_node_internal *e = (dom_node_internal *) element;

	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	a = _dom_hash_get(hs, name);

	/* Detach attr node from list */
	if (a != NULL) {
		/* Disptach DOMNodeRemoval event */
		bool success = true;
		struct dom_document *doc = dom_node_get_owner(element);
		err = dom_node_dispatch_node_change_event(doc,
				a, element, DOM_MUTATION_REMOVAL, &success);
		if (err != DOM_NO_ERR)
			return err;

		/* Delete the attribute node */
		_dom_hash_del(hs, name);
		/* Claim a reference for later event dispatch */
		dom_node_ref(a);

		/* And destroy attr */
		dom_node_set_parent(a, NULL);
		dom_node_try_destroy(a);

		/* Dispatch a DOMAttrModified event */
		success = true;
		dom_string *old = NULL;
		err = dom_attr_get_value(a, &old);
		/* TODO: We did not support some node type such as entity
		 * reference, in that case, we should ignore the error to
		 * make sure the event model work as excepted. */
		if (err != DOM_NO_ERR && err != DOM_NOT_SUPPORTED_ERR)
			return err;
		err = _dom_dispatch_attr_modified_event(doc, e, old, NULL, a,
				name, DOM_MUTATION_REMOVAL, &success);
		dom_string_unref(old);
		/* Release the reference */
		dom_node_unref(a);
		if (err != DOM_NO_ERR)
			return err;

		success = true;
		err = _dom_dispatch_subtree_modified_event(doc,
				(dom_event_target *) e, &success);
		if (err != DOM_NO_ERR)
			return err;
	}

	/** \todo defaulted attribute handling */

	return DOM_NO_ERR;
}

/**
 * Retrieve an attribute node from an element by name
 *
 * \param element  The element to retrieve attribute node from
 * \param name     The attribute's name
 * \param result   Pointer to location to receive attribute node
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_get_attr_node(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name, 
		struct dom_attr **result)
{
	void *a;

	UNUSED(element);

	a = _dom_hash_get(hs, name);

	/* Fill in value */
	if (a == NULL) {
		*result = NULL;
	} else {
		*result = (dom_attr *) a;
		dom_node_ref(*result);
	}

	return DOM_NO_ERR;
}

/**
 * Set an attribute node on an element, replacing existing node, if present
 *
 * \param element  The element to add a node to
 * \param attr     The attribute node to add
 * \param result   Pointer to location to receive previous node
 * \return DOM_NO_ERR                      on success,
 *         DOM_WRONG_DOCUMENT_ERR          if ::attr does not belong to the
 *                                         same document as ::element,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_INUSE_ATTRIBUTE_ERR         if ::attr is already an attribute
 *                                         of another Element node.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_set_attr_node(struct dom_element *element,
		struct dom_hash_table *hs, struct dom_attr *attr, 
		struct dom_attr **result)
{
	dom_exception err;
	dom_string *name = NULL;
	bool added;
	dom_node_internal *e = (dom_node_internal *) element;
	dom_node_internal *a = (dom_node_internal *) attr;

	/** \todo validate name */

	/* Ensure element and attribute belong to the same document */
	if (e->owner != a->owner)
		return DOM_WRONG_DOCUMENT_ERR;

	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Ensure attribute isn't attached to another element */
	if (a->parent != NULL && a->parent != e)
		return DOM_INUSE_ATTRIBUTE_ERR;

	err = dom_node_get_local_name(attr, &name);
	if (err != DOM_NO_ERR)
		return err;

	a = _dom_hash_get(hs, name);

	*result = NULL;
	if (a != NULL) {
		/* Disptach DOMNodeRemoval event */
		bool success = true;
		struct dom_document *doc = dom_node_get_owner(element);
		err = dom_node_dispatch_node_change_event(doc,
				a, element, DOM_MUTATION_REMOVAL, &success);
		if (err != DOM_NO_ERR) {
			dom_string_unref(name);
			return err;
		}

		_dom_hash_del(hs, name);
		dom_node_ref(a);
		*result = (dom_attr *) a;
		dom_node_set_parent(a, NULL);
		dom_node_mark_pending(a);

		/* Dispatch a DOMAttrModified event */
		dom_string *old = NULL;
		success = true;
		err = dom_attr_get_value(a, &old);
		/* TODO: We did not support some node type such as entity
		 * reference, in that case, we should ignore the error to
		 * make sure the event model work as excepted. */
		if (err != DOM_NO_ERR && err != DOM_NOT_SUPPORTED_ERR) {
			dom_node_unref(a);
			dom_string_unref(name);
			return err;
		}
		err = _dom_dispatch_attr_modified_event(doc, e, old, NULL, 
				(dom_event_target *) a, name, 
				DOM_MUTATION_REMOVAL, &success);
		dom_string_unref(old);
		if (err != DOM_NO_ERR) {
			dom_string_unref(name);
			return err;
		}

		success = true;
		err = _dom_dispatch_subtree_modified_event(doc,
				(dom_event_target *) e, &success);
		if (err != DOM_NO_ERR) {
			dom_string_unref(name);
			return err;
		}
	}	

	added = _dom_hash_add(hs, name, attr, false);
	if (added == false) {
		dom_string_unref(name);
		/* If we failed at this step, there must be no memory */
		return DOM_NO_MEM_ERR;
	}
	dom_string_ref(name);
	dom_node_set_parent(attr, element);
	dom_node_remove_pending(attr);

	/* Dispatch a DOMAttrModified event */
	dom_string *new = NULL;
	struct dom_document *doc = dom_node_get_owner(element);
	bool success = true;
	err = dom_attr_get_value(attr, &new);
	/* TODO: We did not support some node type such as entity reference, in
	 * that case, we should ignore the error to make sure the event model
	 * work as excepted. */
	if (err != DOM_NO_ERR && err != DOM_NOT_SUPPORTED_ERR)
		return err;
	err = _dom_dispatch_attr_modified_event(doc, e, NULL, new,
			(dom_event_target *) attr, name, 
			DOM_MUTATION_ADDITION, &success);
	/* Cleanup */
	dom_string_unref(new);
	dom_string_unref(name);
	if (err != DOM_NO_ERR) {
		return err;
	}

	err = dom_node_dispatch_node_change_event(doc, attr, element, 
			DOM_MUTATION_ADDITION, &success);
	if (err != DOM_NO_ERR)
		return err;

	success = true;
	err = _dom_dispatch_subtree_modified_event(doc,
			(dom_event_target *) element, &success);
	if (err != DOM_NO_ERR)
		return err;

	return DOM_NO_ERR;
}

/**
 * Remove an attribute node from an element
 *
 * \param element  The element to remove attribute node from
 * \param attr     The attribute node to remove
 * \param result   Pointer to location to receive attribute node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::element is readonly,
 *         DOM_NOT_FOUND_ERR               if ::attr is not an attribute of
 *                                         ::element.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_element_remove_attr_node(struct dom_element *element,
		struct dom_hash_table *hs, struct dom_attr *attr, 
		struct dom_attr **result)
{
	void *a;
	dom_exception err;
	dom_string *name;
	dom_node_internal *e = (dom_node_internal *) element;
	
	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	
	err = dom_node_get_node_name(attr, &name);
	if (err != DOM_NO_ERR)
		return err;

	a = _dom_hash_get(hs, name);

	/** \todo defaulted attribute handling */

	if (a == NULL || a != (void *) attr) {
		dom_string_unref(name);
		return DOM_NOT_FOUND_ERR;
	}

	/* Dispatch a DOMNodeRemoved event */
	bool success = true;
	struct dom_document *doc = dom_node_get_owner(element);
	err = dom_node_dispatch_node_change_event(doc, a, element, 
			DOM_MUTATION_REMOVAL, &success);
	if (err != DOM_NO_ERR) {
		dom_string_unref(name);
		return err;
	}

	/* Delete the attribute node */
	_dom_hash_del(hs, name);
	dom_node_ref(a);

	/* Now, cleaup the dom_string */
	dom_string_unref(name);

	/* Dispatch a DOMAttrModified event */
	dom_string *old = NULL;
	success = true;
	err = dom_attr_get_value(a, &old);
	/* TODO: We did not support some node type such as entity reference, in
	 * that case, we should ignore the error to make sure the event model
	 * work as excepted. */
	if (err != DOM_NO_ERR && err != DOM_NOT_SUPPORTED_ERR) {
		dom_node_unref(a);
		return err;
	}
	err = _dom_dispatch_attr_modified_event(doc, e, old, NULL, 
			(dom_event_target *) a, name, 
			DOM_MUTATION_REMOVAL, &success);
	dom_string_unref(old);
	if (err != DOM_NO_ERR)
		return err;

	/* When a Node is removed, it should be destroy. When its refcnt is not 
	 * zero, it will be added to the document's deletion pending list. 
	 * When a Node is removed, its parent should be NULL, but its owner
	 * should remain to be the document.
	 */
	*result = (dom_attr *) a;
	dom_node_set_parent(a, NULL);
	dom_node_mark_pending(a);

	success = true;
	err = _dom_dispatch_subtree_modified_event(doc, (dom_event_target *) e,
			&success);
	if (err != DOM_NO_ERR)
		return err;

	return DOM_NO_ERR;
}

/**
 * Test whether certain attribute is inside the hash table
 *
 * \param element  The element
 * \param hs       The hash table contains the attributes
 * \param name     The attribute's name
 * \param result   The return value
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_has_attr(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name,
		bool *result)
{
	void *a;

	UNUSED(element);

	a = _dom_hash_get(hs, name);

	/* Fill in value */
	if (a == NULL) {
		*result = false;
	} else {
		*result = true;
	}

	return DOM_NO_ERR;
}

/**
 * (Un)set an attribute Node as a ID.
 *
 * \param element  The element contains the attribute
 * \param hs       The hash table which contains the attribute node
 * \param name     The name of the attribute
 * \param is_id    true for set the node as a ID attribute, false unset it
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_set_id_attr(struct dom_element *element,
		struct dom_hash_table *hs, dom_string *name, bool is_id)
{
	dom_attr *attr;
	struct dom_hash_table *oh;

	attr = _dom_hash_get(hs, name);
	if (attr == NULL)
		return DOM_NOT_FOUND_ERR;
	
	if (is_id == true) {
		/* Firstly, clear the previous id attribute if there is one */
		if (element->id_ns != NULL) {
			assert(element->id_name != NULL);
			oh = _dom_hash_get(element->ns_attributes, 
					element->id_ns);
		} else {
			oh = element->attributes;
		}
		assert(oh != NULL);

		if (element->id_name != NULL) {
			attr = _dom_hash_get(oh, element->id_name);
			assert(attr != NULL);
			_dom_attr_set_isid(attr, false);
		}
	}

	_dom_attr_set_isid(attr, is_id);

	element->id_name = dom_string_ref(name);

	return DOM_NO_ERR;
}

/**
 * Get the ID string of the element
 *
 * \param ele  The element
 * \param id   The ID of this element
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_element_get_id(struct dom_element *ele, dom_string **id)
{
	dom_exception err;
	dom_string *ret = NULL;

	*id = NULL;

	if (ele->id_ns != NULL && ele->id_name != NULL) {
		/* There is user specific ID attribute */
		err = _dom_element_get_attribute_ns(ele, ele->id_ns, 
				ele->id_name, &ret);
		if (err != DOM_NO_ERR) {
			return err;
		}

		*id = ret;
		return err;
	}

	dom_document *doc;
	doc = dom_node_get_owner(ele);
	assert(doc != NULL);
	dom_string *name;

	if (ele->id_name != NULL) {
		name = ele->id_name;
	} else {
		name = _dom_document_get_id_name(doc);

		if (name == NULL) {
			/* No ID attribute at all, just return NULL */
			*id = NULL;
			return DOM_NO_ERR;
		}
	}

	err = _dom_element_get_attribute(ele, name, &ret);
	if (err != DOM_NO_ERR) {
		return err;
	}

	if (ret != NULL) {
		*id = ret;
	} else {
		*id = NULL;
	}

	return err;
}



/*-------------- The dom_namednodemap functions -------------------------*/

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_get_length(void *priv,
		unsigned long *length)
{
	unsigned int ret = 0;
	uintptr_t c1, *c2 = NULL;
	void *key, *value;
	dom_element *e = (dom_element *) priv;

	ret += _dom_hash_get_length(e->attributes);
	while( (key = _dom_hash_iterate(e->ns_attributes, &c1, &c2)) != NULL) {
		value = _dom_hash_get(e->ns_attributes, key);
		if (value != NULL) {
			ret += _dom_hash_get_length(
					(struct dom_hash_table *) value);
		}
	}

	*length = ret;
	return DOM_NO_ERR;
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_get_named_item(void *priv,
		dom_string *name, struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;

	return _dom_element_get_attribute_node(e, name, (dom_attr **) node);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_set_named_item(void *priv,
		struct dom_node *arg, struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;
	dom_node_internal *n = (dom_node_internal *) arg;

	if (n->type != DOM_ATTRIBUTE_NODE)
		return DOM_HIERARCHY_REQUEST_ERR;

	return _dom_element_set_attribute_node(e, (dom_attr *) arg,
			(dom_attr **) node);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_remove_named_item(
		void *priv, dom_string *name,
		struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;
	dom_exception err;

	err = _dom_element_get_attribute_node(e, name, (dom_attr **) node);
	if (err != DOM_NO_ERR)
		return err;

	if (*node == NULL) {
		return DOM_NOT_FOUND_ERR;
	}
	
	return _dom_element_remove_attribute(e, name);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_item(void *priv,
		unsigned long index, struct dom_node **node)
{
	struct dom_hash_table *ht = NULL;
	unsigned int num = index + 1;
	unsigned int len;
	dom_element *e = (dom_element *) priv;
	void *key, *value;
	uintptr_t c1, *c2 = NULL;

	len = _dom_hash_get_length(e->attributes);
	if (num <= len) {
		ht = e->attributes;
	} else {
		num -= len;
	}

	while( (key = _dom_hash_iterate(e->ns_attributes, &c1, &c2)) != NULL) {
		value = _dom_hash_get(e->ns_attributes, key);
		if (value != NULL) {
			len = _dom_hash_get_length(
					(struct dom_hash_table *) value);
			if (num <= len) {
				ht = (struct dom_hash_table *) value;
				break;
			} else {
				num -= len;
			}
		}
	}

	*node = NULL;
	c2 = NULL;
	if (ht != NULL)
	{
		while( (key = _dom_hash_iterate(ht, &c1, &c2)) != NULL) {
			value = _dom_hash_get(ht, key);
			if (--num == 0) {
				*node = (dom_node *) value;
				break;
			}
		}
	}

	if (*node != NULL)
		dom_node_ref(*node);

	return DOM_NO_ERR;
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_get_named_item_ns(
		void *priv, dom_string *namespace,
		dom_string *localname, struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;

	return _dom_element_get_attribute_node_ns(e, namespace, localname,
			(dom_attr **) node);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_set_named_item_ns(
		void *priv, struct dom_node *arg,
		struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;
	dom_node_internal *n = (dom_node_internal *) arg;

	if (n->type != DOM_ATTRIBUTE_NODE)
		return DOM_HIERARCHY_REQUEST_ERR;

	return _dom_element_set_attribute_node_ns(e, (dom_attr *) arg,
			(dom_attr **) node);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
dom_exception attributes_remove_named_item_ns(
		void *priv, dom_string *namespace,
		dom_string *localname, struct dom_node **node)
{
	dom_element *e = (dom_element *) priv;
	dom_exception err;
	
	err = _dom_element_get_attribute_node_ns(e, namespace, localname,
			(dom_attr **) node);
	if (err != DOM_NO_ERR)
		return err;

	if (*node == NULL) {
		return DOM_NOT_FOUND_ERR;
	}

	return _dom_element_remove_attribute_ns(e, namespace, localname);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
void attributes_destroy(void *priv)
{
	dom_element *e = (dom_element *) priv;

	dom_node_unref(e);
}

/* Implementation function for NamedNodeMap, see core/namednodemap.h for 
 * details */
bool attributes_equal(void *p1, void *p2)
{
	/* We have passed the pointer to this element as the private data,
	 * and here we just need to compare whether the two elements are 
	 * equal
	 */
	return p1 == p2;
}
/*------------------ End of namednodemap functions -----------------------*/

uint32_t attributes_hash(void *key, void *pw)
{
	UNUSED(pw);

	return dom_string_hash(key);
}

void *attributes_clone_key(void *key, void *pw)
{
	UNUSED(pw);

	assert(key != NULL);

	return dom_string_ref(key);
}

void attributes_destroy_key(void *key, void *pw)
{
	UNUSED(pw);

	assert(key != NULL);

	dom_string_unref(key);
}

void *attributes_clone_value(void *value, void *pw)
{
	dom_exception err;
	dom_node *node;

	UNUSED(pw);

	assert(value != NULL);

	err = dom_node_clone_node(value, true, &node);
	if (err != DOM_NO_ERR)
		return NULL;

	return node;
}

void attributes_destroy_value(void *value, void *pw)
{
	dom_node_internal *a = (dom_node_internal *) value;

	UNUSED(pw);

	assert(value != NULL);

	a->parent = NULL;
	dom_node_try_destroy(a);
}

void *attributes_ns_clone_value(void *value, void *pw)
{
	UNUSED(pw);

	assert(value != NULL);

	return _dom_hash_clone(value);
}

void attributes_ns_destroy_value(void *value, void *pw)
{
	UNUSED(pw);

	assert(value != NULL);

	_dom_hash_destroy(value);
}

bool attributes_key_isequal(void *key1, void *key2, void *pw)
{
	UNUSED(pw);

	return dom_string_isequal(key1, key2);
}
