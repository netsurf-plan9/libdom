/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <stdio.h>

#include <dom/core/attr.h>
#include <dom/core/element.h>
#include <dom/core/node.h>
#include <dom/core/string.h>

#include "core/attr.h"
#include "core/document.h"
#include "core/element.h"
#include "core/node.h"
#include "utils/namespace.h"
#include "utils/utils.h"

static struct dom_element_vtable element_vtable = {
	{
		DOM_NODE_VTABLE
	},
	DOM_ELEMENT_VTABLE
};

/**
 * Initialise an element node
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
dom_exception dom_element_initialise(struct dom_element *el,
		struct dom_string *name, struct dom_string *namespace,
		struct dom_string *prefix, struct dom_element **result)
{
	dom_exception err;
	struct dom_document *doc;

	dom_node_get_owner_document(el, &doc);

	/** \todo Sanity check the tag name */

	/* Initialise the base class */
	err = dom_node_initialise(&el->base, doc, DOM_ELEMENT_NODE,
			name, NULL, namespace, prefix);
	if (err != DOM_NO_ERR) {
		dom_document_alloc(doc, el, 0);
		return err;
	}

	/* Perform our type-specific initialisation */
	el->attributes = NULL;
	el->schema_type_info = NULL;

	/* Init the vtable's destroy function */
	el->base.base.vtable = &element_vtable;
	el->base.destroy = &_dom_element_destroy;

	*result = el;

	return DOM_NO_ERR;
}

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
dom_exception dom_element_create(struct dom_document *doc,
		struct dom_string *name, struct dom_string *namespace,
		struct dom_string *prefix, struct dom_element **result)
{
	struct dom_element *el;

	/* Allocate the element */
	el = dom_document_alloc(doc, NULL, sizeof(struct dom_element));
	if (el == NULL)
		return DOM_NO_MEM_ERR;

	dom_element_initialise(el, name, namespace, prefix, result);
	
	return DOM_NO_ERR;
}

/**
 * Destroy an element
 *
 * \param doc      The owning document
 * \param element  The element to destroy
 *
 * The contents of ::element will be destroyed and ::element will be freed.
 */
void dom_element_destroy(struct dom_document *doc,
		struct dom_element *element)
{
	struct dom_node_internal *c, *d;

	/* Destroy children of this node */
	for (c = element->base.first_child; c != NULL; c = d) {
		d = c->next;

		/* Detach child */
		c->parent = NULL;

		if (c->refcnt > 0) {
			/* Something is using this child */

			/** \todo add to list of nodes pending deletion */

			continue;
		}

		/* Detach from sibling list */
		c->previous = NULL;
		c->next = NULL;

		dom_node_destroy(c);
	}

	/* Destroy attributes attached to this node */
	for (c = (struct dom_node_internal *) element->attributes;
			c != NULL; c = d) {
		d = c->next;

		/* Detach child */
		c->parent = NULL;

		if (c->refcnt > 0) {
			/* Something is using this attribute */

			/** \todo add to list of nodes pending deletion */

			continue;
		}

		/* Detach from sibling list */
		c->previous = NULL;
		c->next = NULL;

		dom_node_destroy(c);
	}

	if (element->schema_type_info != NULL) {
		/** \todo destroy schema type info */
	}

	/* Finalise base class */
	dom_node_finalise(doc, &element->base);

	/* Free the element */
	dom_document_alloc(doc, element, 0);
}

/**
 * The destroy virtual function of dom_element 
 * 
 * \param element The element to be destroyed
 **/
void _dom_element_destroy(struct dom_node_internal *node)
{
	struct dom_document *doc;
	dom_node_get_owner_document(node, &doc);

	dom_element_destroy(doc, (struct dom_element *) node);
}

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
		struct dom_string **name)
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
		struct dom_string *name, struct dom_string **value)
{
	struct dom_node_internal *a = (struct dom_node_internal *) 
			element->attributes;

	/* Search attributes, looking for name */
	for (; a != NULL; a = a->next) {
		if (dom_string_cmp(a->name, name) == 0)
			break;
	}

	/* Fill in value */
	if (a == NULL) {
		*value = NULL;
	} else {
		dom_attr_get_value(((struct dom_attr *) a), value);
	}

	return DOM_NO_ERR;
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
		struct dom_string *name, struct dom_string *value)
{
	struct dom_node_internal *e = (struct dom_node_internal *) element;
	struct dom_node_internal *a = (struct dom_node_internal *) 
			element->attributes;

	/** \todo validate name */

	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Search for existing attribute with same name */
	for (; a != NULL; a = a->next) {
		if (dom_string_cmp(a->name, name) == 0)
			break;
	}

	if (a != NULL) {
		/* Found an existing attribute, so replace its value */
		dom_exception err;

		err = dom_attr_set_value((struct dom_attr *) a, value);
		if (err != DOM_NO_ERR)
			return err;
	} else {
		/* No existing attribute, so create one */
		dom_exception err;
		struct dom_attr *attr;

		err = dom_attr_create(e->owner, name, NULL, NULL, &attr);
		if (err != DOM_NO_ERR)
			return err;

		/* Set its value */
		err = dom_attr_set_value(attr, value);
		if (err != DOM_NO_ERR) {
			dom_node_unref((struct dom_node *) attr);
			return err;
		}

		a = (struct dom_node_internal *) attr;

		/* And insert it into the element */
		a->previous = NULL;
		a->next = (struct dom_node_internal *) element->attributes;

		if (a->next != NULL)
			a->next->previous = a;

		element->attributes = attr;
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
dom_exception _dom_element_remove_attribute(struct dom_element *element,
		struct dom_string *name)
{
	struct dom_node_internal *e = (struct dom_node_internal *) element;
	struct dom_node_internal *a = (struct dom_node_internal *) 
			element->attributes;

	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Search for existing attribute with same name */
	for (; a != NULL; a = a->next) {
		if (dom_string_cmp(a->name, name) == 0)
			break;
	}

	/* Detach attr node from list */
	if (a != NULL) {
		if (a->previous != NULL)
			a->previous->next = a->next;
		else
			element->attributes = (struct dom_attr *) a->next;

		if (a->next != NULL)
			a->next->previous = a->previous;

		a->previous = a->next = a->parent = NULL;

		/* And destroy attr */
		dom_node_unref(a);
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
dom_exception _dom_element_get_attribute_node(struct dom_element *element,
		struct dom_string *name, struct dom_attr **result)
{
	struct dom_node_internal *a = (struct dom_node_internal *) 
			element->attributes;

	/* Search attributes, looking for name */
	for (; a != NULL; a = a->next) {
		if (dom_string_cmp(a->name, name) == 0)
			break;
	}

	if (a != NULL)
		dom_node_ref(a);
	*result = (struct dom_attr *) a;

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
dom_exception _dom_element_set_attribute_node(struct dom_element *element,
		struct dom_attr *attr, struct dom_attr **result)
{
	struct dom_node_internal *e = (struct dom_node_internal *) element;
	struct dom_node_internal *a = (struct dom_node_internal *) attr;
	struct dom_attr *prev = NULL;

	/* Ensure element and attribute belong to the same document */
	if (e->owner != a->owner)
		return DOM_WRONG_DOCUMENT_ERR;

	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Ensure attribute isn't attached to another element */
	if (a->parent != NULL && a->parent != e)
		return DOM_INUSE_ATTRIBUTE_ERR;

	/* Attach attr to element, if not already attached */
	if (a->parent == NULL) {

		/* Search for existing attribute with same name */
		prev = element->attributes; 
		while (prev != NULL) {
			struct dom_node_internal *p = 
					(struct dom_node_internal *) prev;

			if (dom_string_cmp(a->name, p->name) == 0)
				break;

			prev = (struct dom_attr *) p->next;
		}

		a->parent = e;

		if (prev != NULL) {
			/* Found an existing attribute, so replace it */
			struct dom_node_internal *p = 
					(struct dom_node_internal *) prev;

			a->previous = p->previous;
			a->next = p->next;

			if (a->previous != NULL)
				a->previous->next = a;
			else
				element->attributes = attr;

			if (a->next != NULL)
				a->next->previous = a;

			/* Invalidate existing attribute's location info */
			p->next = NULL;
			p->previous = NULL;
			p->parent = NULL;
		} else {
			/* No existing attribute, so insert at front of list */
			a->previous = NULL;
			a->next = (struct dom_node_internal *) 
					element->attributes;

			if (a->next != NULL)
				a->next->previous = a;

			element->attributes = attr;
		}
	}

	if (prev != NULL)
		dom_node_ref((struct dom_node *) prev);

	*result = prev;

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
dom_exception _dom_element_remove_attribute_node(struct dom_element *element,
		struct dom_attr *attr, struct dom_attr **result)
{
	struct dom_node_internal *e = (struct dom_node_internal *) element;
	struct dom_node_internal *a = (struct dom_node_internal *) attr;

	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Ensure attr is an attribute of element */
	if (a->parent != e)
		return DOM_NOT_FOUND_ERR;

	/* Detach attr node from list */
	if (a->previous != NULL)
		a->previous->next = a->next;
	else
		element->attributes = (struct dom_attr *) a->next;

	if (a->next != NULL)
		a->next->previous = a->previous;

	a->previous = a->next = a->parent = NULL;

	/** \todo defaulted attribute handling */

	/* Return the detached node */
	dom_node_ref(a);
	*result = attr;

	return DOM_NO_ERR;
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
		struct dom_element *element, struct dom_string *name,
		struct dom_nodelist **result)
{
	return dom_document_get_nodelist(element->base.owner, 
			(struct dom_node_internal *) element, name, NULL, 
			NULL, result);
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
		struct dom_string *namespace, struct dom_string *localname,
		struct dom_string **value)
{
	struct dom_node_internal *a = (struct dom_node_internal *) 
			element->attributes;

	/** \todo ensure implementation supports XML */

	/* Search attributes, looking for namespace/localname pair */
	for (; a != NULL; a = a->next) {
		if (((namespace == NULL && a->namespace == NULL) || 
			(namespace != NULL && 
			dom_string_cmp(a->namespace, namespace) == 0)) &&
				dom_string_cmp(a->name, localname) == 0)
			break;
	}

	/* Fill in value */
	if (a == NULL) {
		*value = NULL;
	} else {
		dom_attr_get_value(((struct dom_attr *) a), value);
	}

	return DOM_NO_ERR;
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
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_string *value)
{
	struct dom_node_internal *e = (struct dom_node_internal *) element;
	struct dom_node_internal *a = (struct dom_node_internal *) 
			element->attributes;
	struct dom_string *prefix, *localname;
	dom_exception err;

	/** \todo ensure XML feature is supported */

	/* Validate name */
	err = _dom_namespace_validate_qname(qname, namespace);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Ensure element can be written to */
	if (_dom_node_readonly(e)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	/* Decompose QName */
	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Search for existing attribute with same namespace/localname */
	for (; a != NULL; a = a->next) {
		if (((namespace == NULL && a->namespace == NULL) || 
			(namespace != NULL && 
			dom_string_cmp(a->namespace, namespace) == 0)) &&
				dom_string_cmp(a->name, localname) == 0)
			break;
	}

	if (a != NULL) {
		/* Found an existing attribute, so replace its prefix & value */
		dom_exception err;

		err = dom_node_set_prefix(a, prefix);
		if (err != DOM_NO_ERR) {
			if (prefix != NULL) {
				dom_string_unref(prefix);
			}
			dom_string_unref(localname);
			return err;
		}

		err = dom_attr_set_value((struct dom_attr *) a, value);
		if (err != DOM_NO_ERR) {
			if (prefix != NULL) {
				dom_string_unref(prefix);
			}
			dom_string_unref(localname);
			return err;
		}
	} else {
		/* No existing attribute, so create one */
		dom_exception err;
		struct dom_attr *attr;

		err = dom_attr_create(e->owner, localname, 
				namespace, prefix, &attr);
		if (err != DOM_NO_ERR) {
			if (prefix != NULL) {
				dom_string_unref(prefix);
			}
			dom_string_unref(localname);
			return err;
		}

		/* Set its value */
		err = dom_attr_set_value(attr, value);
		if (err != DOM_NO_ERR) {
			dom_node_unref((struct dom_node *) attr);

			if (prefix != NULL) {
				dom_string_unref(prefix);
			}
			dom_string_unref(localname);
			return err;
		}

		a = (struct dom_node_internal *) attr;

		/* And insert it into the element */
		a->previous = NULL;
		a->next = (struct dom_node_internal *) element->attributes;

		if (a->next != NULL)
			a->next->previous = a;

		element->attributes = attr;
	}

	return DOM_NO_ERR;
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
		struct dom_string *namespace, struct dom_string *localname)
{
	struct dom_node_internal *e = (struct dom_node_internal *) element;
	struct dom_node_internal *a = (struct dom_node_internal *) 
			element->attributes;

	/** \todo ensure XML feature is supported */

	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Search for existing attribute with same namespace/localname */
	for (; a != NULL; a = a->next) {
		if (((namespace == NULL && a->namespace == NULL) || 
			(namespace != NULL && 
			dom_string_cmp(a->namespace, namespace) == 0)) &&
				dom_string_cmp(a->name, localname) == 0)
			break;
	}

	/* Detach attr node from list */
	if (a != NULL) {
		if (a->previous != NULL)
			a->previous->next = a->next;
		else
			element->attributes = (struct dom_attr *) a->next;

		if (a->next != NULL)
			a->next->previous = a->previous;

		a->previous = a->next = a->parent = NULL;

		/* And destroy attr */
		dom_node_unref(a);
	}

	/** \todo defaulted attribute handling */

	return DOM_NO_ERR;
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
		struct dom_string *namespace, struct dom_string *localname,
		struct dom_attr **result)
{
	struct dom_node_internal *a = (struct dom_node_internal *) 
			element->attributes;

	/** \todo ensure XML feature is supported */

	/* Search attributes, looking for namespace/localname */
	for (; a != NULL; a = a->next) {
		if (((namespace == NULL && a->namespace == NULL) || 
			(namespace != NULL && 
			dom_string_cmp(a->namespace, namespace) == 0)) &&
				dom_string_cmp(a->name, localname) == 0)
			break;
	}

	if (a != NULL)
		dom_node_ref(a);
	*result = (struct dom_attr *) a;

	return DOM_NO_ERR;
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
	struct dom_node_internal *e = (struct dom_node_internal *) element;
	struct dom_node_internal *a = (struct dom_node_internal *) attr;
	struct dom_attr *prev = NULL;

	/** \todo ensure XML feature is supported */

	/* Ensure element and attribute belong to the same document */
	if (e->owner != a->owner)
		return DOM_WRONG_DOCUMENT_ERR;

	/* Ensure element can be written to */
	if (_dom_node_readonly(e))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;

	/* Ensure attribute isn't attached to another element */
	if (a->parent != NULL && a->parent != e)
		return DOM_INUSE_ATTRIBUTE_ERR;

	/* Attach attr to element, if not already attached */
	if (a->parent == NULL) {

		/* Search for existing attribute with same namespace/localname */
		prev = element->attributes; 
		while (prev != NULL) {
			struct dom_node_internal *p = 
					(struct dom_node_internal *) prev;

			if (((a->namespace == NULL && p->namespace == NULL) || 
				(a->namespace != NULL && 
				dom_string_cmp(a->namespace, 
						p->namespace) == 0)) &&
				dom_string_cmp(a->name, p->name) == 0)
			break;

			prev = (struct dom_attr *) p->next;
		}

		a->parent = e;

		if (prev != NULL) {
			/* Found an existing attribute, so replace it */
			struct dom_node_internal *p = 
					(struct dom_node_internal *) prev;

			a->previous = p->previous;
			a->next = p->next;

			if (a->previous != NULL)
				a->previous->next = a;
			else
				element->attributes = attr;

			if (a->next != NULL)
				a->next->previous = a;

			/* Invalidate existing attribute's location info */
			p->next = NULL;
			p->previous = NULL;
			p->parent = NULL;
		} else {
			/* No existing attribute, so insert at front of list */
			a->previous = NULL;
			a->next = (struct dom_node_internal *) 
					element->attributes;

			if (a->next != NULL)
				a->next->previous = a;

			element->attributes = attr;
		}
	}

	if (prev != NULL)
		dom_node_ref((struct dom_node *) prev);

	*result = prev;

	return DOM_NO_ERR;
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
		struct dom_element *element, struct dom_string *namespace,
		struct dom_string *localname, struct dom_nodelist **result)
{
	/** \todo ensure XML feature is supported */

	return dom_document_get_nodelist(element->base.owner, 
			(struct dom_node_internal *) element, NULL, 
			namespace, localname, result);
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
		struct dom_string *name, bool *result)
{
	struct dom_node_internal *a = (struct dom_node_internal *) 
			element->attributes;

	/* Search attributes, looking for name */
	for (; a != NULL; a = a->next) {
		if (dom_string_cmp(a->name, name) == 0)
			break;
	}

	*result = (a != NULL);

	return DOM_NO_ERR;
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
		struct dom_string *namespace, struct dom_string *localname,
		bool *result)
{
	struct dom_node_internal *a = (struct dom_node_internal *) 
			element->attributes;

	/** \todo ensure XML feature is supported */

	/* Search attributes, looking for namespace/localname */
	for (; a != NULL; a = a->next) {
		if (((namespace == NULL && a->namespace == NULL) || 
			(namespace != NULL && 
			dom_string_cmp(a->namespace, namespace) == 0)) &&
				dom_string_cmp(a->name, localname) == 0)
			break;
	}

	*result = (a != NULL);

	return DOM_NO_ERR;
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
 */
dom_exception _dom_element_set_id_attribute(struct dom_element *element,
		struct dom_string *name, bool is_id)
{
	UNUSED(element);
	UNUSED(name);
	UNUSED(is_id);

	return DOM_NOT_SUPPORTED_ERR;
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
		struct dom_string *namespace, struct dom_string *localname,
		bool is_id)
{
	UNUSED(element);
	UNUSED(namespace);
	UNUSED(localname);
	UNUSED(is_id);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(element);
	UNUSED(id_attr);
	UNUSED(is_id);

	return DOM_NOT_SUPPORTED_ERR;
}

/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */

/**
 * Retrieve a map of attributes associated with an Element
 *
 * \param element  The element to retrieve the attributes of
 * \param result   Pointer to location to receive attribute map
 * \return DOM_NO_ERR.
 *
 * The returned NamedNodeMap will be referenced. It is the responsibility
 * of the caller to unref the map once it has finished with it.
 */
dom_exception dom_element_get_attributes(struct dom_element *element,
		struct dom_namednodemap **result)
{
	return dom_document_get_namednodemap(element->base.owner,
			(struct dom_node_internal *) element, 
			DOM_ATTRIBUTE_NODE, result);
}

/**
 * Determine if an element has any attributes
 *
 * \param element  Element to inspect
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_element_has_attributes(struct dom_element *element,
		bool *result)
{
	*result = (element->attributes != NULL);

	return DOM_NO_ERR;
}

/**
 * Retrieve a pointer to the first attribute attached to an element
 *
 * \param element  The element to retrieve the first attribute from
 * \return Pointer to first attribute, or NULL if none.
 */
struct dom_node_internal *dom_element_get_first_attribute(
		struct dom_element *element)
{
	return (struct dom_node_internal *) element->attributes;
}

