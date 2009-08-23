/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <stddef.h>
#include <string.h>
#include <assert.h>

#include <dom/core/attr.h>
#include <dom/core/document.h>
#include <dom/core/node.h>
#include <dom/core/string.h>

#include "core/attr.h"
#include "core/document.h"
#include "core/entity_ref.h"
#include "core/node.h"
#include "utils/utils.h"

struct dom_element;

/**
 * DOM node attribute
 */
struct dom_attr {
	struct dom_node_internal base;	/**< Base node */

	bool specified;		/**< Whether the attribute is specified
				 * or default */

	struct dom_type_info *schema_type_info;	/**< Type information */

	bool is_id;	/**< Whether this attribute is a ID attribute */
};

/* The vtable for dom_attr node */
static struct dom_attr_vtable attr_vtable = {
	{
		DOM_NODE_VTABLE_ATTR
	},
	DOM_ATTR_VTABLE
};

/* The protected vtable for dom_attr */
static struct dom_node_protect_vtable attr_protect_vtable = {
	DOM_ATTR_PROTECT_VTABLE
};


/* -------------------------------------------------------------------- */

/* Constructor and destructor */

/**
 * Create an attribute node
 *
 * \param doc        The owning document
 * \param name       The (local) name of the node to create
 * \param namespace  The namespace URI of the attribute, or NULL
 * \param prefix     The namespace prefix of the attribute, or NULL
 * \param specified  Whtether this attribute is specified
 * \param result     Pointer to location to receive created attribute
 * \return DOM_NO_ERR     on success,
 *         DOM_NO_MEM_ERR on memory exhaustion.
 *
 * ::doc and ::name will have their reference counts increased. The 
 * caller should make sure that ::name is a valid NCName here.
 *
 * The returned attribute will already be referenced.
 */
dom_exception _dom_attr_create(struct dom_document *doc,
		struct lwc_string_s *name, struct lwc_string_s *namespace,
		struct lwc_string_s *prefix, bool specified, 
		struct dom_attr **result)
{
	struct dom_attr *a;
	dom_exception err;

	/* Allocate the attribute node */
	a = _dom_document_alloc(doc, NULL, sizeof(struct dom_attr));
	if (a == NULL)
		return DOM_NO_MEM_ERR;

	/* Initialise the vtable */
	a->base.base.vtable = &attr_vtable;
	a->base.vtable = &attr_protect_vtable;

	/* Initialise the class */
	err = _dom_attr_initialise(a, doc, name, namespace, prefix, specified, 
			result);
	if (err != DOM_NO_ERR) {
		_dom_document_alloc(doc, a, 0);
		return err;
	}
	return DOM_NO_ERR;
}

/**
 * Initialise a dom_attr
 *
 * \param a          The dom_attr
 * \param doc        The document
 * \param name       The name of this attribute node
 * \param namespace  The namespace of this attribute
 * \param prefix     The prefix
 * \param specified  Whether this node is a specified one
 * \param result     The returned node
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_attr_initialise(dom_attr *a, 
		struct dom_document *doc,  struct lwc_string_s *name,
		struct lwc_string_s *namespace, struct lwc_string_s *prefix,
		bool specified, struct dom_attr **result)
{
	dom_exception err;

	err = _dom_node_initialise(&a->base, doc, DOM_ATTRIBUTE_NODE,
			name, NULL, namespace, prefix);
	if (err != DOM_NO_ERR) {
		return err;
	}

	a->specified = specified;
	a->schema_type_info = NULL;
	a->is_id = false;

	*result = a;

	return DOM_NO_ERR;
}

/**
 * The destructor of dom_attr
 *
 * \param doc   The owner document
 * \param attr  The attribute
 */
void _dom_attr_finalise(dom_document *doc, dom_attr *attr)
{
	/* Now, clean up this node and destroy it */

	if (attr->schema_type_info != NULL) {
		/** \todo destroy schema type info */
	}

	_dom_node_finalise(doc, &attr->base);
}

/**
 * Destroy an attribute node
 *
 * \param doc   The owning document
 * \param attr  The attribute to destroy
 *
 * The contents of ::attr will be destroyed and ::attr will be freed
 */
void _dom_attr_destroy(struct dom_document *doc, struct dom_attr *attr)
{
	_dom_attr_finalise(doc, attr);

	_dom_document_alloc(doc, attr, 0);
}


/* -------------------------------------------------------------------- */

/* The public virtual functions */

/**
 * Retrieve an attribute's name
 *
 * \param attr    Attribute to retrieve name from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_attr_get_name(struct dom_attr *attr,
		struct dom_string **result)
{
	/* This is the same as nodeName */
	return dom_node_get_node_name(attr, result);
}

/**
 * Determine if attribute was specified or default
 *
 * \param attr    Attribute to inspect
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_attr_get_specified(struct dom_attr *attr, bool *result)
{
	*result = attr->specified;

	return DOM_NO_ERR;
}

/**
 * Retrieve an attribute's value
 *
 * \param attr    Attribute to retrieve value from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_attr_get_value(struct dom_attr *attr,
		struct dom_string **result)
{
	struct dom_node_internal *a = (struct dom_node_internal *) attr;
	struct dom_node_internal *c;
	struct dom_string *value, *temp;
	dom_exception err;

	err = _dom_document_create_string(a->owner, 
			NULL, 0, &value);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Traverse children, building a string representation as we go */
	for (c = a->first_child; c != NULL; c = c->next) {
		if (c->type == DOM_TEXT_NODE && c->value != NULL) {
			/* Append to existing value */
			err = dom_string_concat(value, c->value, &temp);
			if (err != DOM_NO_ERR) {
				dom_string_unref(value);
				return err;
			}

			/* Finished with previous value */
			dom_string_unref(value);

			/* Claim new value */
			value = temp;
		} else if (c->type == DOM_ENTITY_REFERENCE_NODE) {
			struct dom_string *tr;

			/* Get textual representation of entity */
			err = _dom_entity_reference_get_textual_representation(
					(struct dom_entity_reference *) c,
					&tr);
			if (err != DOM_NO_ERR) {
				dom_string_unref(value);
				return err;
			}

			/* Append to existing value */
			err = dom_string_concat(value, tr, &temp);
			if (err != DOM_NO_ERR) {
				dom_string_unref(tr);
				dom_string_unref(value);
				return err;
			}

			/* No longer need textual representation */
			dom_string_unref(tr);

			/* Finished with previous value */
			dom_string_unref(value);

			/* Claim new value */
			value = temp;
		}
	}

	*result = value;

	return DOM_NO_ERR;
}

/**
 * Set an attribute's value
 *
 * \param attr   Attribute to retrieve value from
 * \param value  New value for attribute
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if attribute is readonly.
 */
dom_exception _dom_attr_set_value(struct dom_attr *attr,
		struct dom_string *value)
{
	struct dom_node_internal *a = (struct dom_node_internal *) attr;
	struct dom_node_internal *c, *d;
	struct dom_text *text;
	dom_exception err;

	/* Ensure attribute is writable */
	if (_dom_node_readonly(a))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Create text node containing new value */
	err = dom_document_create_text_node(a->owner, value, &text);
	if (err != DOM_NO_ERR)
		return err;

	/* Destroy children of this node */
	for (c = a->first_child; c != NULL; c = d) {
		d = c->next;

		/* Detach child */
		c->parent = NULL;

		/* Detach from sibling list */
		c->previous = NULL;
		c->next = NULL;

		dom_node_try_destroy(c);
	}

	/* And insert the text node as the value */
	((struct dom_node_internal *) text)->parent = a;
	a->first_child = a->last_child = (struct dom_node_internal *) text;
	dom_node_unref(text);
	dom_node_remove_pending(text);

	/* Now the attribute node is specified */
	attr->specified = true;

	return DOM_NO_ERR;
}

/**
 * Retrieve the owning element of an attribute
 *
 * \param attr    The attribute to extract owning element from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. The caller
 * should unref it once it has finished with it.
 */
dom_exception _dom_attr_get_owner(struct dom_attr *attr,
		struct dom_element **result)
{
	struct dom_node_internal *a = (struct dom_node_internal *) attr;

	/* If there is an owning element, increase its reference count */
	if (a->parent != NULL)
		dom_node_ref(a->parent);

	*result = (struct dom_element *) a->parent;

	return DOM_NO_ERR;
}

/**
 * Retrieve an attribute's type information
 *
 * \param attr    The attribute to extract type information from
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 *
 * The returned type info will have its reference count increased. The caller
 * should unref it once it has finished with it.
 */
dom_exception _dom_attr_get_schema_type_info(struct dom_attr *attr,
		struct dom_type_info **result)
{
	UNUSED(attr);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Determine if an attribute if of type ID
 *
 * \param attr    The attribute to inspect
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_attr_is_id(struct dom_attr *attr, bool *result)
{
	*result = attr->is_id;

	return DOM_NO_ERR;
}

/*------------- The overload virtual functions ------------------------*/

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_get_node_value(dom_node_internal *node,
		struct dom_string **result)
{
	dom_attr *attr = (dom_attr *) node;

	return _dom_attr_get_value(attr, result);
}

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_clone_node(dom_node_internal *node, bool deep,
		dom_node_internal **result)
{
	dom_exception err;
	dom_attr *attr;

	/* Discard the warnings */
	UNUSED(deep);

	/* Clone an Attr always clone all its children */
	err = _dom_node_clone_node(node, true, result);
	if (err != DOM_NO_ERR)
		return err;
	
	attr = (dom_attr *) *result;
	/* Clone an Attr always result a specified Attr, 
	 * see DOM Level 3 Node.cloneNode */
	attr->specified = true;

	return DOM_NO_ERR;
}

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_set_prefix(dom_node_internal *node,
		struct dom_string *prefix)
{
	/* Really I don't know whether there should something
	 * special to do here */
	return _dom_node_set_prefix(node, prefix);
}

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_lookup_prefix(dom_node_internal *node,
		struct dom_string *namespace, struct dom_string **result)
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

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_is_default_namespace(dom_node_internal *node,
		struct dom_string *namespace, bool *result)
{
	struct dom_element *owner;
	dom_exception err;

	err = dom_attr_get_owner_element(node, &owner);
	if (err != DOM_NO_ERR)
		return err;
	
	if (owner == NULL) {
		*result = false;
		return DOM_NO_ERR;
	}

	return dom_node_is_default_namespace(owner, namespace, result);
}

/* Overload function of Node, please refer node.c for the detail of this 
 * function. */
dom_exception _dom_attr_lookup_namespace(dom_node_internal *node,
		struct dom_string *prefix, struct dom_string **result)
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

	return dom_node_lookup_namespace(owner, prefix, result);
}


/*----------------------------------------------------------------------*/

/* The protected virtual functions */

/* The virtual destroy function of this class */
void __dom_attr_destroy(dom_node_internal *node)
{
	dom_document *doc = node->owner;

	assert(doc != NULL);
	_dom_attr_destroy(doc, (dom_attr *) node);
}

/* The memory allocator of this class */
dom_exception _dom_attr_alloc(struct dom_document *doc, 
		struct dom_node_internal *n, struct dom_node_internal **ret)
{
	UNUSED(n);
	dom_attr *a;
	
	a = _dom_document_alloc(doc, NULL, sizeof(struct dom_attr));
	if (a == NULL)
		return DOM_NO_MEM_ERR;
	
	*ret = (dom_node_internal *) a;
	dom_node_set_owner(*ret, doc);

	return DOM_NO_ERR;
}

/* The copy constructor of this class  */
dom_exception _dom_attr_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old)
{
	dom_attr *na = (dom_attr *) new;
	dom_attr *oa = (dom_attr *) old;

	na->specified = oa->specified;
	na->is_id = oa->is_id;

	/* TODO: deal with dom_type_info, it get no definition ! */

	return _dom_node_copy(new, old);
}


/**
 * Set/Unset whether this attribute is a ID attribute 
 *
 * \param attr   The attribute
 * \param is_id  Whether it is a ID attribute
 */
void _dom_attr_set_isid(struct dom_attr *attr, bool is_id)
{
	attr->is_id = is_id;
}

/**
 * Set/Unset whether the attribute is a specified one.
 *
 * \param attr       The attribute node
 * \param specified  Whether this attribute is a specified one
 */
void _dom_attr_set_specified(struct dom_attr *attr, bool specified)
{
	attr->specified = specified;
}

