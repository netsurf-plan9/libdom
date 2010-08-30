/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>

#include <libwapcaplet/libwapcaplet.h>

#include <dom/functypes.h>
#include <dom/bootstrap/implpriv.h>
#include <dom/core/attr.h>
#include <dom/core/element.h>
#include <dom/core/document.h>
#include <dom/core/implementation.h>

#include "core/string.h"
#include "core/attr.h"
#include "core/cdatasection.h"
#include "core/comment.h"
#include "core/document.h"
#include "core/doc_fragment.h"
#include "core/element.h"
#include "core/entity_ref.h"
#include "core/namednodemap.h"
#include "core/nodelist.h"
#include "core/pi.h"
#include "core/text.h"
#include "utils/validate.h"
#include "utils/namespace.h"
#include "utils/utils.h"

/**
 * Item in list of active nodelists
 */
struct dom_doc_nl {
	struct dom_nodelist *list;	/**< Nodelist */

	struct dom_doc_nl *next;	/**< Next item */
	struct dom_doc_nl *prev;	/**< Previous item */
};

/* The virtual functions of this dom_document */
static struct dom_document_vtable document_vtable = {
	{
		DOM_NODE_VTABLE
	},
	DOM_DOCUMENT_VTABLE
};

static struct dom_node_protect_vtable document_protect_vtable = {
	DOM_DOCUMENT_PROTECT_VTABLE
};


/*----------------------------------------------------------------------*/

/* Internally used helper functions */
static dom_exception dom_document_dup_node(dom_document *doc, 
		struct dom_node *node, bool deep, struct dom_node **result, 
		dom_node_operation opt);


/*----------------------------------------------------------------------*/

/* The constructors and destructors */

/**
 * Create a Document
 *
 * \param impl   The DOM implementation owning the document
 * \param alloc  Memory (de)allocation function
 * \param pw     Pointer to client-specific private data
 * \param doc    Pointer to location to receive created document
 * \param daf    The default action fetcher
 * \param daf    The default action fetcher
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion.
 *
 * ::impl will have its reference count increased.
 *
 * The returned document will already be referenced.
 */
dom_exception _dom_document_create(struct dom_implementation *impl,
		dom_alloc alloc, void *pw,
		dom_events_default_action_fetcher daf,
		struct dom_document **doc)
{
	struct dom_document *d;
	dom_exception err;

	/* Create document */
	d = alloc(NULL, sizeof(struct dom_document), pw);
	if (d == NULL)
		return DOM_NO_MEM_ERR;

	/* Initialise the virtual table */
	d->base.base.vtable = &document_vtable;
	d->base.vtable = &document_protect_vtable;

	/* Initialise base class -- the Document has no parent, so
	 * destruction will be attempted as soon as its reference count
	 * reaches zero. Documents own themselves (this simplifies the 
	 * rest of the code, as it doesn't need to special case Documents)
	 */
	err = _dom_document_initialise(d, impl, alloc, pw, daf);
	if (err != DOM_NO_ERR) {
		/* Clean up document */
		alloc(d, 0, pw);
		return err;
	}

	*doc = d;

	return DOM_NO_ERR;
}

/* Initialise the document */
dom_exception _dom_document_initialise(struct dom_document *doc, 
		struct dom_implementation *impl, dom_alloc alloc, void *pw, 
		dom_events_default_action_fetcher daf)
{
	assert(alloc != NULL);
	assert(impl != NULL);

	dom_exception err;
	lwc_string *name;
	lwc_error lerr;
	
	lerr = lwc_intern_string("#document", SLEN("#document"), &name);
	if (lerr != lwc_error_ok)
		return _dom_exception_from_lwc_error(lerr);

	dom_implementation_ref(impl);
	doc->impl = impl;

	doc->nodelists = NULL;

	/* Set up document allocation context - must be first */
	doc->alloc = alloc;
	doc->pw = pw;

	err = _dom_node_initialise(&doc->base, doc, DOM_DOCUMENT_NODE,
			name, NULL, NULL, NULL);
	lwc_string_unref(name);

	list_init(&doc->pending_nodes);

	doc->id_name = NULL;

	/* We should not pass a NULL when all things hook up */
	return _dom_document_event_internal_initialise(doc, &doc->dei, daf);
}


/* Finalise the document */
bool _dom_document_finalise(struct dom_document *doc)
{
	/* Finalise base class, delete the tree in force */
	_dom_node_finalise(doc, &doc->base);

	/* Now, the first_child and last_child should be null */
	doc->base.first_child = NULL;
	doc->base.last_child = NULL;

	/* Ensure list of nodes pending deletion is empty. If not,
	 * then we can't yet destroy the document (its destruction will
	 * have to wait until the pending nodes are destroyed) */
	if (doc->pending_nodes.next != &doc->pending_nodes)
		return false;

	/* Ok, the document tree is empty, as is the list of nodes pending
	 * deletion. Therefore, it is safe to destroy the document. */
	if (doc->impl != NULL)
		dom_implementation_unref(doc->impl);
	doc->impl = NULL;

	/* This is paranoia -- if there are any remaining nodelists,
	 * then the document's reference count will be
	 * non-zero as these data structures reference the document because
	 * they are held by the client. */
	doc->nodelists = NULL;

	if (doc->id_name != NULL)
		lwc_string_unref(doc->id_name);
	
	_dom_document_event_internal_finalise(doc, &doc->dei);
	
	_dom_document_event_internal_finalise(doc, &doc->dei);

	return true;
}



/*----------------------------------------------------------------------*/

/* Public virtual functions */

/**
 * Retrieve the doctype of a document
 *
 * \param doc     The document to retrieve the doctype from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_get_doctype(struct dom_document *doc,
		struct dom_document_type **result)
{
	struct dom_node_internal *c;

	for (c = doc->base.first_child; c != NULL; c = c->next) {
		if (c->type == DOM_DOCUMENT_TYPE_NODE)
			break;
	}

	if (c != NULL)
		dom_node_ref(c);

	*result = (struct dom_document_type *) c;

	return DOM_NO_ERR;
}

/**
 * Retrieve the DOM implementation that handles this document
 *
 * \param doc     The document to retrieve the implementation from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned implementation will have its reference count increased.
 * It is the responsibility of the caller to unref the implementation once
 * it has finished with it.
 */
dom_exception _dom_document_get_implementation(struct dom_document *doc,
		struct dom_implementation **result)
{
	if (doc->impl != NULL)
		dom_implementation_ref(doc->impl);

	*result = doc->impl;

	return DOM_NO_ERR;
}

/**
 * Retrieve the document element of a document
 *
 * \param doc     The document to retrieve the document element from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_get_document_element(struct dom_document *doc,
		struct dom_element **result)
{
	struct dom_node_internal *root;

	/* Find the first element node in child list */
	for (root = doc->base.first_child; root != NULL; root = root->next) {
		if (root->type == DOM_ELEMENT_NODE)
			break;
	}

	if (root != NULL)
		dom_node_ref(root);

	*result = (struct dom_element *) root;

	return DOM_NO_ERR;
}

/**
 * Create an element
 *
 * \param doc       The document owning the element
 * \param tag_name  The name of the element
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::tag_name is invalid.
 *
 * ::doc and ::tag_name will have their reference counts increased.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_element(struct dom_document *doc,
		struct dom_string *tag_name, struct dom_element **result)
{
	lwc_string *name;
	dom_exception err;

	if (_dom_validate_name(tag_name) == false)
		return DOM_INVALID_CHARACTER_ERR;

	err = _dom_string_intern(tag_name, &name);
	if (err != DOM_NO_ERR)
		return err;
	
	err = _dom_element_create(doc, name, NULL, NULL, result);
	lwc_string_unref(name);

	return err;
}

/**
 * Create a document fragment
 *
 * \param doc     The document owning the fragment
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_document_fragment(struct dom_document *doc,
		struct dom_document_fragment **result)
{
	lwc_string *name;
	dom_exception err;
	lwc_error lerr;

	lerr = lwc_intern_string("#document-fragment", 
			SLEN("#document-fragment"), &name);
	if (lerr != lwc_error_ok)
		return _dom_exception_from_lwc_error(lerr);
	
	err = _dom_document_fragment_create(doc, name, NULL, result);
	lwc_string_unref(name);

	return err;
}

/**
 * Create a text node
 *
 * \param doc     The document owning the node
 * \param data    The data for the node
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_text_node(struct dom_document *doc,
		struct dom_string *data, struct dom_text **result)
{
	lwc_string *name;
	dom_exception err;
	lwc_error lerr;

	lerr = lwc_intern_string("#text", SLEN("#text"), &name);
	if (lerr != lwc_error_ok)
		return _dom_exception_from_lwc_error(lerr);
	
	err = _dom_text_create(doc, name, data, result);
	lwc_string_unref(name);

	return err;
}

/**
 * Create a comment node
 *
 * \param doc     The document owning the node
 * \param data    The data for the node
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_comment(struct dom_document *doc,
		struct dom_string *data, struct dom_comment **result)
{
	lwc_string *name;
	dom_exception err;
	lwc_error lerr;

	lerr = lwc_intern_string("#comment", SLEN("#comment"),
			&name);
	if (lerr != lwc_error_ok)
		return _dom_exception_from_lwc_error(lerr);
	
	err = _dom_comment_create(doc, name, data, result);
	lwc_string_unref(name);

	return err;
}

/**
 * Create a CDATA section
 *
 * \param doc     The document owning the section
 * \param data    The data for the section contents
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if this is an HTML document.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_cdata_section(struct dom_document *doc,
		struct dom_string *data, struct dom_cdata_section **result)
{
	lwc_string *name;
	dom_exception err;
	lwc_error lerr;

	lerr = lwc_intern_string("#cdata-section", 
			SLEN("#cdata-section"), &name);
	if (lerr != lwc_error_ok)
		return _dom_exception_from_lwc_error(lerr);

	err = _dom_cdata_section_create(doc, name, data, result);
	lwc_string_unref(name);

	return err;
}

/**
 * Create a processing instruction
 *
 * \param doc     The document owning the instruction
 * \param target  The instruction target
 * \param data    The data for the node
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::target is invalid,
 *         DOM_NOT_SUPPORTED_ERR     if this is an HTML document.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_processing_instruction(
		struct dom_document *doc, struct dom_string *target,
		struct dom_string *data,
		struct dom_processing_instruction **result)
{
	lwc_string *name;
	dom_exception err;

	if (_dom_validate_name(target) == false)
		return DOM_INVALID_CHARACTER_ERR;

	err = _dom_string_intern(target, &name);
	if (err != DOM_NO_ERR)
		return err;

	err = _dom_processing_instruction_create(doc, name, data, result);
	lwc_string_unref(name);

	return err;
}

/**
 * Create an attribute
 *
 * \param doc     The document owning the attribute
 * \param name    The name of the attribute
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::name is invalid.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_attribute(struct dom_document *doc,
		struct dom_string *name, struct dom_attr **result)
{
	lwc_string *n;
	dom_exception err;

	if (_dom_validate_name(name) == false)
		return DOM_INVALID_CHARACTER_ERR;

	err = _dom_string_intern(name, &n);
	if (err != DOM_NO_ERR)
		return err;

	err = _dom_attr_create(doc, n, NULL, NULL, true, result);
	lwc_string_unref(n);
	return err;
}

/**
 * Create an entity reference
 *
 * \param doc     The document owning the reference
 * \param name    The name of the entity to reference
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::name is invalid,
 *         DOM_NOT_SUPPORTED_ERR     if this is an HTML document.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_entity_reference(struct dom_document *doc,
		struct dom_string *name,
		struct dom_entity_reference **result)
{
	lwc_string *n;
	dom_exception err;

	if (_dom_validate_name(name) == false)
		return DOM_INVALID_CHARACTER_ERR;

	err = _dom_string_intern(name, &n);
	if (err != DOM_NO_ERR)
		return err;

	err = _dom_entity_reference_create(doc, n, NULL, result);
	lwc_string_unref(n);
	return err;
}

/**
 * Retrieve a list of all elements with a given tag name
 *
 * \param doc      The document to search in
 * \param tagname  The tag name to search for ("*" for all)
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned list will have its reference count increased. It is
 * the responsibility of the caller to unref the list once it has
 * finished with it.
 */
dom_exception _dom_document_get_elements_by_tag_name(struct dom_document *doc,
		struct dom_string *tagname, struct dom_nodelist **result)
{
	lwc_string *name;
	dom_exception err;

	err = _dom_string_intern(tagname, &name);
	if (err != DOM_NO_ERR)
		return err;

	err = _dom_document_get_nodelist(doc, DOM_NODELIST_BY_NAME, 
			(struct dom_node_internal *) doc,  name, NULL, NULL, 
			result);
	lwc_string_unref(name);

	return err;
}

/**
 * Import a node from another document into this one
 *
 * \param doc     The document to import into
 * \param node    The node to import
 * \param deep    Whether to copy the node's subtree
 * \param result  Pointer to location to receive imported node in this document.
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if any of the names are invalid,
 *         DOM_NOT_SUPPORTED_ERR     if the type of ::node is unsupported
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_import_node(struct dom_document *doc,
		struct dom_node *node, bool deep, struct dom_node **result)
{
	/* TODO: The DOM_INVALID_CHARACTER_ERR exception */

	return dom_document_dup_node(doc, node, deep, result,
			DOM_NODE_IMPORTED);
}

/**
 * Create an element from the qualified name and namespace URI
 *
 * \param doc        The document owning the element
 * \param namespace  The namespace URI to use, or NULL for none
 * \param qname      The qualified name of the element
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::qname is invalid,
 *         DOM_NAMESPACE_ERR         if ::qname is malformed, or it has a
 *                                   prefix and ::namespace is NULL, or
 *                                   ::qname has a prefix "xml" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or ::qname has a prefix "xmlns" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/2000/xmlns", or
 *                                   ::namespace is
 *                                   "http://www.w3.org/2000/xmlns" and
 *                                   ::qname is not (or is not prefixed by)
 *                                   "xmlns",
 *         DOM_NOT_SUPPORTED_ERR     if ::doc does not support the "XML"
 *                                   feature.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_element_ns(struct dom_document *doc,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_element **result)
{
	struct dom_string *prefix, *localname;
	dom_exception err;

	if (_dom_validate_name(qname) == false)
		return DOM_INVALID_CHARACTER_ERR;

	/* Validate qname */
	err = _dom_namespace_validate_qname(qname, namespace);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Divide QName into prefix/localname pair */
	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Get the interned string from the dom_string */
	lwc_string *l = NULL, *n = NULL, *p = NULL;
	if (localname != NULL) {
		err = _dom_string_intern(localname, &l);
		if (err != DOM_NO_ERR) {
			dom_string_unref(localname);
			if (prefix != NULL)
				dom_string_unref(prefix);

			return err;
		}
	}
	if (namespace != NULL) {
		err = _dom_string_intern(namespace, &n);
		if (err != DOM_NO_ERR) {
			lwc_string_unref(l);
			dom_string_unref(localname);
			if (prefix != NULL)
				dom_string_unref(prefix);

			return err;
		}
	}
	if (prefix != NULL) {
		err = _dom_string_intern(prefix, &p);
		if (err != DOM_NO_ERR) {
			lwc_string_unref(l);
			lwc_string_unref(n);
			dom_string_unref(localname);
			if (prefix != NULL)
				dom_string_unref(prefix);

			return err;
		}
	}

	/* Attempt to create element */
	err = _dom_element_create(doc, l, n, p, result);

	/* Tidy up */
	if (localname != NULL) {
		dom_string_unref(localname);
		lwc_string_unref(l);
	}
	if (prefix != NULL) {
		dom_string_unref(prefix);
		lwc_string_unref(p);
	}
	if (namespace != NULL) {
		lwc_string_unref(n);
	}

	return err;
}

/**
 * Create an attribute from the qualified name and namespace URI
 *
 * \param doc        The document owning the attribute
 * \param namespace  The namespace URI to use
 * \param qname      The qualified name of the attribute
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::qname is invalid,
 *         DOM_NAMESPACE_ERR         if ::qname is malformed, or it has a
 *                                   prefix and ::namespace is NULL, or
 *                                   ::qname has a prefix "xml" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or ::qname has a prefix "xmlns" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/2000/xmlns", or
 *                                   ::namespace is
 *                                   "http://www.w3.org/2000/xmlns" and
 *                                   ::qname is not (or is not prefixed by)
 *                                   "xmlns",
 *         DOM_NOT_SUPPORTED_ERR     if ::doc does not support the "XML"
 *                                   feature.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_create_attribute_ns(struct dom_document *doc,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_attr **result)
{
	struct dom_string *prefix, *localname;
	dom_exception err;

	if (_dom_validate_name(qname) == false)
		return DOM_INVALID_CHARACTER_ERR;

	/* Validate qname */
	err = _dom_namespace_validate_qname(qname, namespace);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Divide QName into prefix/localname pair */
	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Get the interned string from the dom_string */
	lwc_string *l = NULL, *n = NULL, *p = NULL;
	if (localname != NULL) {
		err = _dom_string_intern(localname, &l);
		if (err != DOM_NO_ERR) {
			dom_string_unref(localname);
			if (prefix != NULL)
				dom_string_unref(prefix);

			return err;
		}
	}
	if (namespace != NULL) {
		err = _dom_string_intern(namespace, &n);
		if (err != DOM_NO_ERR) {
			lwc_string_unref(l);
			dom_string_unref(localname);
			if (prefix != NULL)
				dom_string_unref(prefix);

			return err;
		}
	}
	if (prefix != NULL) {
		err = _dom_string_intern(prefix, &p);
		if (err != DOM_NO_ERR) {
			lwc_string_unref(l);
			lwc_string_unref(n);
			dom_string_unref(localname);
			if (prefix != NULL)
				dom_string_unref(prefix);

			return err;
		}
	}
	/* Attempt to create attribute */
	err = _dom_attr_create(doc, l, n, p, true, result);

	/* Tidy up */
	if (localname != NULL) {
		dom_string_unref(localname);
		lwc_string_unref(l);
	}
	if (prefix != NULL) {
		dom_string_unref(prefix);
		lwc_string_unref(p);
	}
	if (namespace != NULL) {
		lwc_string_unref(n);
	}

	return err;
}

/**
 * Retrieve a list of all elements with a given local name and namespace URI
 *
 * \param doc        The document to search in
 * \param namespace  The namespace URI
 * \param localname  The local name
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 *
 * The returned list will have its reference count increased. It is
 * the responsibility of the caller to unref the list once it has
 * finished with it.
 */
dom_exception _dom_document_get_elements_by_tag_name_ns(
		struct dom_document *doc, struct dom_string *namespace,
		struct dom_string *localname, struct dom_nodelist **result)
{
	dom_exception err;
	lwc_string *l = NULL, *n = NULL;

	/* Get the interned string from the dom_string */
	if (localname != NULL) {
		err = _dom_string_intern(localname, &l);
		if (err != DOM_NO_ERR)
			return err;
	}
	if (namespace != NULL) {
		err = _dom_string_intern(namespace, &n);
		if (err != DOM_NO_ERR) {
			lwc_string_unref(l);
			return err;
		}
	}

	err = _dom_document_get_nodelist(doc, DOM_NODELIST_BY_NAMESPACE, 
			(struct dom_node_internal *) doc, NULL, n, l, result);
	
	if (l != NULL)
		lwc_string_unref(l);
	if (n != NULL)
		lwc_string_unref(n);

	return err;
}

/**
 * Retrieve the element that matches the specified ID
 *
 * \param doc     The document to search in
 * \param id      The ID to search for
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception _dom_document_get_element_by_id(struct dom_document *doc,
		struct dom_string *id, struct dom_element **result)
{
	lwc_string *i;
	dom_node_internal *root;
	dom_exception err;

	*result = NULL;

	err = _dom_string_intern(id, &i);
	if (err != DOM_NO_ERR)
		return err;
	
	err = dom_document_get_document_element(doc, (void *) &root);
	if (err != DOM_NO_ERR)
		return err;

	err = _dom_find_element_by_id(root, i, result);
	dom_node_unref(root);

	return err;
}

/**
 * Retrieve the input encoding of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_get_input_encoding(struct dom_document *doc,
		struct dom_string **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the XML encoding of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_get_xml_encoding(struct dom_document *doc,
		struct dom_string **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the standalone status of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 */
dom_exception _dom_document_get_xml_standalone(struct dom_document *doc,
		bool *result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set the standalone status of the document
 *
 * \param doc         The document to query
 * \param standalone  Standalone status to use
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the document does not support the "XML"
 *                               feature.
 *
 * We don't support this API now, so the return value is always 
 * DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_set_xml_standalone(struct dom_document *doc,
		bool standalone)
{
	UNUSED(doc);
	UNUSED(standalone);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the XML version of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * We don't support this API now, so the return value is always 
 * DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_get_xml_version(struct dom_document *doc,
		struct dom_string **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set the XML version of the document
 *
 * \param doc      The document to query
 * \param version  XML version to use
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the document does not support the "XML"
 *                               feature.
 *
 * We don't support this API now, so the return value is always 
 * DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_set_xml_version(struct dom_document *doc,
		struct dom_string *version)
{
	UNUSED(doc);
	UNUSED(version);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the error checking mode of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 */
dom_exception _dom_document_get_strict_error_checking(
		struct dom_document *doc, bool *result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set the error checking mode of the document
 *
 * \param doc     The document to query
 * \param strict  Whether to use strict error checking
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 */
dom_exception _dom_document_set_strict_error_checking(
		struct dom_document *doc, bool strict)
{
	UNUSED(doc);
	UNUSED(strict);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the URI of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_get_uri(struct dom_document *doc,
		struct dom_string **result)
{
	dom_string_ref(doc->uri);
	*result = doc->uri;

	return DOM_NO_ERR;
}

/**
 * Set the URI of the document
 *
 * \param doc  The document to query
 * \param uri  The URI to use
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_set_uri(struct dom_document *doc,
		struct dom_string *uri)
{
	dom_string_unref(doc->uri);
	dom_string_ref(uri);
	doc->uri = uri;

	return DOM_NO_ERR;
}

/**
 * Attempt to adopt a node from another document into this document
 *
 * \param doc     The document to adopt into
 * \param node    The node to adopt
 * \param result  Pointer to location to receive adopted node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly,
 *         DOM_NOT_SUPPORTED_ERR           if ::node is of type Document or
 *                                         DocumentType
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 *
 * @note: The spec said adoptNode may be light weight than the importNode
 *	  because the former need no Node creation. But in our implementation
 *	  this can't be ensured. Both adoptNode and importNode create new
 *	  nodes using the importing/adopting document's resource manager. So,
 *	  generally, the adoptNode and importNode call the same function
 *	  dom_document_dup_node.
 */
dom_exception _dom_document_adopt_node(struct dom_document *doc,
		struct dom_node *node, struct dom_node **result)
{
	dom_exception err;
	dom_node_internal *n = (dom_node_internal *) node;
	
	*result = NULL;

	if (n->type == DOM_DOCUMENT_NODE ||
			n->type == DOM_DOCUMENT_TYPE_NODE) {
		return DOM_NOT_SUPPORTED_ERR;		
	}

	if (n->type == DOM_ENTITY_NODE ||
			n->type == DOM_NOTATION_NODE ||
			n->type == DOM_PROCESSING_INSTRUCTION_NODE ||
			n->type == DOM_TEXT_NODE ||
			n->type == DOM_CDATA_SECTION_NODE ||
			n->type == DOM_COMMENT_NODE) {
		*result = NULL;
		return DOM_NO_ERR;
	}

	/* Support XML when necessary */
	if (n->type == DOM_ENTITY_REFERENCE_NODE) {
		return DOM_NOT_SUPPORTED_ERR;
	}

	err = dom_document_dup_node(doc, node, true, result, DOM_NODE_ADOPTED);
	if (err != DOM_NO_ERR) {
		*result = NULL;
		return err;
	}

	dom_node_internal *parent = n->parent;
	dom_node_internal *tmp;
	if (parent != NULL) {
		err = dom_node_remove_child(parent, node, (void *) &tmp);
		if (err != DOM_NO_ERR) {
			dom_node_unref(*result);
			*result = NULL;
			return err;
		}
	}

	dom_node_unref(tmp);

	return DOM_NO_ERR;
}

/**
 * Retrieve the DOM configuration associated with a document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 *
 * The returned object will have its reference count increased. It is
 * the responsibility of the caller to unref the object once it has
 * finished with it.
 */
dom_exception _dom_document_get_dom_config(struct dom_document *doc,
		struct dom_configuration **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Normalize a document
 *
 * \param doc  The document to normalize
 * \return DOM_NOT_SUPPORTED_ERR, we don't support this API now.
 */
dom_exception _dom_document_normalize(struct dom_document *doc)
{
	UNUSED(doc);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Rename a node in a document
 *
 * \param doc        The document containing the node
 * \param node       The node to rename
 * \param namespace  The new namespace for the node
 * \param qname      The new qualified name for the node
 * \param result     Pointer to location to receive renamed node
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::tag_name is invalid,
 *         DOM_WRONG_DOCUMENT_ERR    if ::node was created in a different
 *                                   document
 *         DOM_NAMESPACE_ERR         if ::qname is malformed, or it has a
 *                                   prefix and ::namespace is NULL, or
 *                                   ::qname has a prefix "xml" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or ::qname has a prefix "xmlns" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/2000/xmlns", or
 *                                   ::namespace is
 *                                   "http://www.w3.org/2000/xmlns" and
 *                                   ::qname is not (or is not prefixed by)
 *                                   "xmlns",
 *         DOM_NOT_SUPPORTED_ERR     if ::doc does not support the "XML"
 *                                   feature.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 *
 * We don't support this API now, so the return value is always 
 * DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_rename_node(struct dom_document *doc,
		struct dom_node *node,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_node **result)
{
	UNUSED(doc);
	UNUSED(node);
	UNUSED(namespace);
	UNUSED(qname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/*-----------------------------------------------------------------------*/

/* Overload protectd virtual functions */

/* The virtual destroy function of this class */
void _dom_document_destroy(struct dom_node_internal *node)
{
	struct dom_document *doc = (struct dom_document *) node;

	if (_dom_document_finalise(doc) == true) {
		doc->alloc(doc, 0, doc->pw);
	}
}

/* The memory allocation function of this class */
dom_exception __dom_document_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret)
{
	UNUSED(n);
	struct dom_document *a;
	
	a = _dom_document_alloc(doc, NULL, sizeof(struct dom_document));
	if (a == NULL)
		return DOM_NO_MEM_ERR;
	
	*ret = (dom_node_internal *) a;
	dom_node_set_owner(*ret, doc);

	return DOM_NO_ERR;
}

/* The copy constructor function of this class */
dom_exception _dom_document_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old)
{
	UNUSED(new);
	UNUSED(old);

	return DOM_NOT_SUPPORTED_ERR;
}


/* ----------------------------------------------------------------------- */

/* Helper functions */
/**
 * Create a DOM string, using a document's allocation context
 *
 * \param doc     The document
 * \param data    Pointer to string data
 * \param len     Length, in bytes, of string
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will already be referenced, so there is no need
 * to explicitly reference it.
 *
 * The string of characters passed in will be copied for use by the
 * returned DOM string.
 */
dom_exception _dom_document_create_string(struct dom_document *doc,
		const uint8_t *data, size_t len, struct dom_string **result)
{
	return dom_string_create(doc->alloc, doc->pw, data, len, result);
}

/**
 * Create a lwc_string 
 * 
 * \param doc     The document object
 * \param data    The raw string data
 * \param len     The raw string length
 * \param result  The resturned lwc_string
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_document_create_lwcstring(struct dom_document *doc,
		const uint8_t *data, size_t len, struct lwc_string_s **result)
{
	lwc_error lerr;

	UNUSED(doc);

	lerr = lwc_intern_string((const char *) data, len, result);
	
	return _dom_exception_from_lwc_error(lerr);
}

/* Unref a lwc_string created by this document */
void _dom_document_unref_lwcstring(struct dom_document *doc,
		struct lwc_string_s *str)
{
	UNUSED(doc);

	lwc_string_unref(str);
}

/* Get the resource manager from the document */
void _dom_document_get_resource_mgr(
		struct dom_document *doc, struct dom_resource_mgr *rm)
{
	rm->alloc = doc->alloc;
	rm->pw = doc->pw;
}

/* Simple accessor for allocator data for this document */
void _dom_document_get_allocator(struct dom_document *doc, dom_alloc *al, 
		void **pw)
{
	*al = doc->alloc;
	*pw = doc->pw;
}
/*
 * Create a dom_string from a lwc_string.
 *
 * \param doc     The document object
 * \param str     The lwc_string object
 * \param result  The retured dom_string
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_document_create_string_from_lwcstring(
		struct dom_document *doc, struct lwc_string_s *str, 
		struct dom_string **result)
{
	return _dom_string_create_from_lwcstring(doc->alloc, doc->pw, 
			str, result);
}

/**
 * Create a hash_table 
 * 
 * \param doc     The dom_document
 * \param chains  The number of chains
 * \param f       The hash function
 * \param ht      The returned hash_table
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_document_create_hashtable(struct dom_document *doc,
		size_t chains, dom_hash_func f, struct dom_hash_table **ht)
{
	struct dom_hash_table *ret;

	ret = _dom_hash_create(chains, f, doc->alloc, doc->pw);
	if (ret == NULL)
		return DOM_NO_MEM_ERR;
	
	*ht = ret;
	return DOM_NO_ERR;
}

/**
 * (De)allocate memory with a document's context
 *
 * \param doc   The document context to allocate from
 * \param ptr   Pointer to data to reallocate, or NULL to alloc new
 * \param size  Required size of data, or 0 to free
 * \return Pointer to allocated data or NULL on failure
 *
 * This call (modulo ::doc) has the same semantics as realloc().
 * It is a thin veneer over the client-provided allocation function.
 */
void *_dom_document_alloc(struct dom_document *doc, void *ptr, size_t size)
{
	return doc->alloc(ptr, size, doc->pw);
}

/**
 * Get a nodelist, creating one if necessary
 *
 * \param doc        The document to get a nodelist for
 * \param type	     The type of the NodeList
 * \param root       Root node of subtree that list applies to
 * \param tagname    Name of nodes in list (or NULL)
 * \param namespace  Namespace part of nodes in list (or NULL)
 * \param localname  Local part of nodes in list (or NULL)
 * \param list       Pointer to location to receive list
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned list will have its reference count increased. It is
 * the responsibility of the caller to unref the list once it has
 * finished with it.
 */
dom_exception _dom_document_get_nodelist(struct dom_document *doc,
		nodelist_type type, struct dom_node_internal *root,
		struct lwc_string_s *tagname, struct lwc_string_s *namespace,
		struct lwc_string_s *localname, struct dom_nodelist **list)
{
	struct dom_doc_nl *l;
	dom_exception err;

	for (l = doc->nodelists; l; l = l->next) {
		if (_dom_nodelist_match(l->list, type, root, tagname,
				namespace, localname))
			break;
	}

	if (l != NULL) {
		/* Found an existing list, so use it */
		dom_nodelist_ref(l->list);
	} else {
		/* No existing list */

		/* Create active list entry */
		l = doc->alloc(NULL, sizeof(struct dom_doc_nl), doc->pw);
		if (l == NULL)
			return DOM_NO_MEM_ERR;

		/* Create nodelist */
		err = _dom_nodelist_create(doc, type, root, tagname, namespace,
				localname, &l->list);
		if (err != DOM_NO_ERR) {
			doc->alloc(l, 0, doc->pw);
			return err;
		}

		/* Add to document's list of active nodelists */
		l->prev = NULL;
		l->next = doc->nodelists;
		if (doc->nodelists)
			doc->nodelists->prev = l;
		doc->nodelists = l;
	}

	/* Note: the document does not claim a reference on the nodelist
	 * If it did, the nodelist's reference count would never reach zero,
	 * and the list would remain indefinitely. This is not a problem as
	 * the list notifies the document of its destruction via
	 * _dom_document_remove_nodelist. */

	*list = l->list;

	return DOM_NO_ERR;
}

/**
 * Remove a nodelist from a document
 *
 * \param doc   The document to remove the list from
 * \param list  The list to remove
 */
void _dom_document_remove_nodelist(struct dom_document *doc,
		struct dom_nodelist *list)
{
	struct dom_doc_nl *l;

	for (l = doc->nodelists; l; l = l->next) {
		if (l->list == list)
			break;
	}

	if (l == NULL) {
		/* This should never happen; we should probably abort here */
		return;
	}

	/* Remove from list */
	if (l->prev != NULL)
		l->prev->next = l->next;
	else
		doc->nodelists = l->next;

	if (l->next != NULL)
		l->next->prev = l->prev;

	/* And free item */
	doc->alloc(l, 0, doc->pw);
}

/**
 * Find element with certain ID in the subtree rooted at root 
 *
 * \param root    The root element from where we start
 * \param id      The ID of the target element
 * \param result  The result element
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_find_element_by_id(dom_node_internal *root, 
		struct lwc_string_s *id, struct dom_element **result)
{
	*result = NULL;
	dom_node_internal *node = root;

	while (node != NULL) {
		if (node->type == DOM_ELEMENT_NODE) {
			lwc_string *real_id;
			_dom_element_get_id((dom_element *) node, &real_id);
			if (real_id == id) {
				*result = (dom_element *) node;
				return DOM_NO_ERR;
			}
		}

		if (node->first_child != NULL) {
			/* Has children */
			node = node->first_child;
		} else if (node->next != NULL) {
			/* No children, but has siblings */
			node = node->next;
		} else {
			/* No children or siblings. 
			 * Find first unvisited relation. */
			struct dom_node_internal *parent = node->parent;

			while (parent != root &&
					node == parent->last_child) {
				node = parent;
				parent = parent->parent;
			}

			node = node->next;
		}
	}

	return DOM_NO_ERR;
}

/**
 * Duplicate a Node
 *
 * \param doc     The documen
 * \param node    The node to duplicate
 * \param deep    Whether to make a deep copy
 * \param result  The returned node
 * \param opt     Whether this is adopt or import operation
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_document_dup_node(dom_document *doc, struct dom_node *node,
		bool deep, struct dom_node **result, dom_node_operation opt)
{
	dom_exception err;
	dom_node_internal *n = (dom_node_internal *) node;

	if (opt == DOM_NODE_ADOPTED && _dom_node_readonly(n))
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	
	if (n->type == DOM_DOCUMENT_NODE ||
			n->type == DOM_DOCUMENT_TYPE_NODE)
		return DOM_NOT_SUPPORTED_ERR;

	err = dom_node_alloc(doc, node, result);
	if (err != DOM_NO_ERR)
		return err;
	
	err = dom_node_copy(*result, node);
	if (err != DOM_NO_ERR) {
		_dom_document_alloc(doc, *result, 0);
		return err;
	}

	if (n->type == DOM_ATTRIBUTE_NODE) {
		_dom_attr_set_specified((dom_attr *) node, true);
		deep = true;
	}

	if (n->type == DOM_ENTITY_REFERENCE_NODE) {
		deep = false;
	}

	if (n->type == DOM_ELEMENT_NODE) {
		/* Specified attributes are copyied but not default attributes,
		 * if the document object hold all the default attributes, we 
		 * have nothing to do here */
	}

	if (opt == DOM_NODE_ADOPTED && (n->type == DOM_ENTITY_NODE ||
			n->type == DOM_NOTATION_NODE)) {
		/* We did not support XML now */
		return DOM_NOT_SUPPORTED_ERR;
	}

	dom_node_internal *child, *r;
	if (deep == true) {
		child = ((dom_node_internal *) node)->first_child;
		while (child != NULL) {
			err = dom_document_import_node(doc, child, deep,
					(void *) &r);
			if (err != DOM_NO_ERR) {
				_dom_document_alloc(doc, *result, 0);
				return err;
			}

			err = dom_node_append_child(*result, r, (void *) &r);
			if (err != DOM_NO_ERR) {
				_dom_document_alloc(doc, *result, 0);
				dom_node_unref(r);
				return err;
			}
			dom_node_unref(r);

			child = child->next;
		}
	}

	/* Call the dom_user_data_handlers */
	dom_user_data *ud;
	ud = n->user_data;
	while (ud != NULL) {
		if (ud->handler != NULL)
			ud->handler(opt, ud->key, ud->data, 
					node, *result);
		ud = ud->next;
	}

	return DOM_NO_ERR;
}

/**
 * Try to destory the document. 
 *
 * \param doc  The instance of Document
 *
 * Delete the document if:
 * 1. The refcnt reach zero
 * 2. The pending list is empty
 *
 * else, do nothing.
 */
void _dom_document_try_destroy(struct dom_document *doc)
{
	if (doc->base.refcnt != 0 || doc->base.parent != NULL)
		return;

	_dom_document_destroy((dom_node_internal *) doc);
}

/**
 * Set the ID attribute name of this document
 *
 * \param doc   The document object
 * \param name  The ID name of the elements in this document
 */
void _dom_document_set_id_name(dom_document *doc, struct lwc_string_s *name)
{
	if (doc->id_name != NULL)
		lwc_string_unref(doc->id_name);
	doc->id_name = lwc_string_ref(name);
}

