/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/bootstrap/implpriv.h>
#include <dom/bootstrap/implregistry.h>
#include <dom/dom.h>

#include "functypes.h"
#include "xmlbinding.h"
#include "utils.h"

static dom_exception xml_dom_get_dom_implementation(
		struct dom_string *features,
		struct dom_implementation **impl, dom_alloc alloc, void *pw);
static dom_exception xml_dom_get_dom_implementation_list(
		struct dom_string *features,
		struct dom_implementation_list **list,
		dom_alloc alloc, void *pw);

static dom_exception xml_dom_implementation_has_feature(
		struct dom_implementation *impl,
		struct dom_string *feature,
		struct dom_string *version,
		bool *result);
static dom_exception xml_dom_implementation_create_document_type(
		struct dom_implementation *impl,
		struct dom_string *qname,
		struct dom_string *public_id,
		struct dom_string *system_id,
		struct dom_document_type **doctype,
		dom_alloc alloc, void *pw);
static dom_exception xml_dom_implementation_create_document(
		struct dom_implementation *impl,
		struct dom_string *namespace,
		struct dom_string *qname,
		struct dom_document_type *doctype,
		struct dom_document **doc,
		dom_alloc alloc, void *pw);
static dom_exception xml_dom_implementation_get_feature(
		struct dom_implementation *impl,
		struct dom_string *feature,
		struct dom_string *version,
		void **object,
		dom_alloc alloc, void *pw);
static void xml_dom_implementation_destroy(struct dom_implementation *impl);


/**
 * DOM implementation source for XML documents
 */
static struct dom_implementation_source xml_dom_impl_src = {
	xml_dom_get_dom_implementation,
	xml_dom_get_dom_implementation_list
};

/**
 * DOM implementation for XML documents
 */
static struct dom_implementation xml_dom_impl = {
	xml_dom_implementation_has_feature,
	xml_dom_implementation_create_document_type,
	xml_dom_implementation_create_document,
	xml_dom_implementation_get_feature,
	xml_dom_implementation_destroy,
	0
};

/**
 * Get a DOM implementation that supports the requested features
 *
 * \param features  String containing required features
 * \param impl      Pointer to location to receive implementation
 * \param alloc     Function to (de)allocate memory
 * \param pw        Pointer to client-specific private data
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function. The implementation's
 * destroy() method will be called once it is no longer used.
 *
 * The implementation will be referenced, so the client need not
 * do this explicitly. The client must unref the implementation
 * once it has finished with it.
 */
dom_exception xml_dom_get_dom_implementation(
		struct dom_string *features,
		struct dom_implementation **impl, dom_alloc alloc, void *pw)
{
	UNUSED(features);
	UNUSED(alloc);
	UNUSED(pw);

	xml_dom_impl.refcnt++;

	*impl = &xml_dom_impl;

	return DOM_NO_ERR;
}

/**
 * Get a list of DOM implementations that support the requested
 * features
 *
 * \param features  String containing required features
 * \param list      Pointer to location to receive list
 * \param alloc     Function to (de)allocate memory
 * \param pw        Pointer to client-specific private data
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function. The ::alloc/::pw
 * pair must be stored on the list object, such that the list
 * and its contents may be freed once they are no longer needed.
 *
 * List nodes reference the implementation objects they point to.
 *
 * The list will be referenced, so the client need not do this
 * explicitly. The client must unref the list once it has finished
 * with it.
 */
dom_exception xml_dom_get_dom_implementation_list(
		struct dom_string *features,
		struct dom_implementation_list **list,
		dom_alloc alloc, void *pw)
{
	struct dom_implementation_list *l;
	struct dom_implementation_list_item *i;

	UNUSED(features);

	l = alloc(NULL, sizeof(struct dom_implementation_list), pw);
	if (l == NULL)
		return DOM_NO_MEM_ERR;

	i = alloc(NULL, sizeof(struct dom_implementation_list_item), pw);
	if (i == NULL) {
		alloc(l, 0, pw);
		return DOM_NO_MEM_ERR;
	}

	i->impl = &xml_dom_impl;
	i->next = NULL;
	i->prev = NULL;

	l->head = i;
	l->alloc = alloc;
	l->pw = pw;

	l->refcnt = 1;

	*list = l;

	return DOM_NO_ERR;
}

/**
 * Test whether a DOM implementation implements a specific feature
 * and version
 *
 * \param impl     The DOM implementation to query
 * \param feature  The feature to test for
 * \param version  The version number of the feature to test for
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception xml_dom_implementation_has_feature(
		struct dom_implementation *impl,
		struct dom_string *feature,
		struct dom_string *version,
		bool *result)
{
	UNUSED(impl);
	UNUSED(feature);
	UNUSED(version);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Create a document type node
 *
 * \param impl       The implementation to create the node
 * \param qname      The qualified name of the document type
 * \param public_id  The external subset public identifier
 * \param system_id  The external subset system identifier
 * \param doctype    Pointer to location to receive result
 * \param alloc      Memory (de)allocation function
 * \param pw         Pointer to client-specific private data
 * \return DOM_NO_ERR on success,
 *         DOM_INVALID_CHARACTER_ERR if ::qname is invalid,
 *         DOM_NAMESPACE_ERR         if ::qname is malformed,
 *         DOM_NOT_SUPPORTED_ERR     if ::impl does not support the
 *                                   feature "XML" and the language
 *                                   exposed through Document does
 *                                   not support XML namespaces.
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function.
 *
 * The doctype will be referenced, so the client need not do this
 * explicitly. The client must unref the doctype once it has
 * finished with it.
 */
dom_exception xml_dom_implementation_create_document_type(
		struct dom_implementation *impl,
		struct dom_string *qname,
		struct dom_string *public_id,
		struct dom_string *system_id,
		struct dom_document_type **doctype,
		dom_alloc alloc, void *pw)
{
	UNUSED(impl);
	UNUSED(qname);
	UNUSED(public_id);
	UNUSED(system_id);
	UNUSED(doctype);
	UNUSED(alloc);
	UNUSED(pw);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Create a document node
 *
 * \param impl       The implementation to create the node
 * \param namespace  The namespace URI of the document element
 * \param qname      The qualified name of the document element
 * \param doctype    The type of document to create
 * \param doc        Pointer to location to receive result
 * \param alloc      Memory (de)allocation function
 * \param pw         Pointer to client-specific private data
 * \return DOM_NO_ERR on success,
 *         DOM_INVALID_CHARACTER_ERR if ::qname is invalid,
 *         DOM_NAMESPACE_ERR         if ::qname is malformed, or if
 *                                   ::qname has a prefix and
 *                                   ::namespace is NULL, or if
 *                                   ::qname is NULL and ::namespace
 *                                   is non-NULL, or if ::qname has
 *                                   a prefix "xml" and ::namespace
 *                                   is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or if ::impl does not support
 *                                   the "XML" feature and
 *                                   ::namespace is non-NULL,
 *         DOM_WRONG_DOCUMENT_ERR    if ::doctype is already being
 *                                   used by a document, or if it
 *                                   was not created by ::impl,
 *         DOM_NOT_SUPPORTED_ERR     if ::impl does not support the
 *                                   feature "XML" and the language
 *                                   exposed through Document does
 *                                   not support XML namespaces.
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function.
 *
 * The document will be referenced, so the client need not do this
 * explicitly. The client must unref the document once it has
 * finished with it.
 */
dom_exception xml_dom_implementation_create_document(
		struct dom_implementation *impl,
		struct dom_string *namespace,
		struct dom_string *qname,
		struct dom_document_type *doctype,
		struct dom_document **doc,
		dom_alloc alloc, void *pw)
{
	struct dom_document *d;
	dom_exception err;

	/* Create document object */
	err = dom_document_create(impl, alloc, pw, &d);
	if (err != DOM_NO_ERR)
		return err;

	/* Set its doctype, if necessary */
	if (doctype != NULL) {
		err = dom_document_set_doctype(d, doctype);
		if (err != DOM_NO_ERR) {
			dom_node_unref((struct dom_node *) d);
			return err;
		}
	}

	/* Create root element and attach it to document */
	if (qname != NULL) {
		struct dom_element *e;
		struct dom_node *inserted;

		err = dom_document_create_element_ns(d, namespace, qname, &e);
		if (err != DOM_NO_ERR) {
			dom_node_unref((struct dom_node *) d);
			return err;
		}

		err = dom_node_append_child((struct dom_node *) d,
				(struct dom_node *) e, &inserted);
		if (err != DOM_NO_ERR) {
			dom_node_unref((struct dom_node *) e);
			dom_node_unref((struct dom_node *) d);
			return err;
		}

		/* No longer interested in inserted node */
		dom_node_unref(inserted);

		/* Done with element */
		dom_node_unref((struct dom_node *) e);
	}

	*doc = d;

	return DOM_NO_ERR;
}

/**
 * Retrieve a specialized object which implements the specified
 * feature and version
 *
 * \param impl     The implementation to create the object
 * \param feature  The requested feature
 * \param version  The version number of the feature
 * \param object   Pointer to location to receive object
 * \param alloc    Memory (de)allocation function
 * \param pw       Pointer to client-specific private data
 * \return DOM_NO_ERR.
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function.
 */
dom_exception xml_dom_implementation_get_feature(
		struct dom_implementation *impl,
		struct dom_string *feature,
		struct dom_string *version,
		void **object,
		dom_alloc alloc, void *pw)
{
	UNUSED(impl);
	UNUSED(feature);
	UNUSED(version);
	UNUSED(object);
	UNUSED(alloc);
	UNUSED(pw);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Destroy a DOM implementation instance
 *
 * \param impl  The instance to destroy
 */
void xml_dom_implementation_destroy(struct dom_implementation *impl)
{
	UNUSED(impl);

	/* Nothing to do -- we're statically allocated */
}

/**
 * Initialise the XML DOM binding
 *
 * \param alloc  Pointer to memory (de)allocation function
 * \param pw     Pointer to client-specific private data
 * \return XML_OK on success, XML_NOMEM on memory exhaustion
 */
xml_error xml_dom_binding_initialise(xml_alloc alloc, void *pw)
{
	dom_exception err;

	err = dom_register_source(&xml_dom_impl_src, (dom_alloc) alloc, pw);
	if (err != DOM_NO_ERR)
		return XML_NOMEM;

	return XML_OK;
}
