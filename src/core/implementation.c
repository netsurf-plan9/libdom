/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/bootstrap/implpriv.h>
#include <dom/core/implementation.h>

/**
 * Claim a reference on a DOM implementation
 *
 * \param impl  The implementation to claim a reference on
 */
void dom_implementation_ref(struct dom_implementation *impl)
{
	impl->refcnt++;
}

/**
 * Release a reference from a DOM implementation
 *
 * \param impl  The implementation to release the reference from
 *
 * If the reference count reaches zero, any memory claimed by the
 * implementation will be released
 */
void dom_implementation_unref(struct dom_implementation *impl)
{
	if (--impl->refcnt == 0) {
		impl->destroy(impl);
	}
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
dom_exception dom_implementation_has_feature(
		struct dom_implementation *impl,
		struct dom_string *feature, struct dom_string *version,
		bool *result)
{
	return impl->has_feature(impl, feature, version, result);
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
 *         DOM_NOT_SUPPORTED_ERR     if ::impl does not support the feature
 *                                   "XML" and the language exposed through
 *                                   Document does not support XML
 *                                   namespaces.
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function.
 *
 * The doctype will be referenced, so the client need not do this
 * explicitly. The client must unref the doctype once it has
 * finished with it.
 */
dom_exception dom_implementation_create_document_type(
		struct dom_implementation *impl, struct dom_string *qname,
		struct dom_string *public_id, struct dom_string *system_id,
		struct dom_document_type **doctype,
		dom_alloc alloc, void *pw)
{
	return impl->create_document_type(impl, qname, public_id, system_id,
			doctype, alloc, pw);
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
 *         DOM_NAMESPACE_ERR         if ::qname is malformed, or if ::qname
 *                                   has a prefix and ::namespace is NULL,
 *                                   or if ::qname is NULL and ::namespace
 *                                   is non-NULL, or if ::qname has a prefix
 *                                   "xml" and ::namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or if ::impl does not support the "XML"
 *                                   feature and ::namespace is non-NULL,
 *         DOM_WRONG_DOCUMENT_ERR    if ::doctype is already being used by a
 *                                   document, or if it was not created by
 *                                   ::impl,
 *         DOM_NOT_SUPPORTED_ERR     if ::impl does not support the feature
 *                                   "XML" and the language exposed through
 *                                   Document does not support XML
 *                                   namespaces.
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function.
 *
 * The document will be referenced, so the client need not do this
 * explicitly. The client must unref the document once it has
 * finished with it.
 */
dom_exception dom_implementation_create_document(
		struct dom_implementation *impl,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_document_type *doctype,
		struct dom_document **doc,
		dom_alloc alloc, void *pw)
{
	return impl->create_document(impl, namespace, qname, doctype, doc,
			alloc, pw);
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
dom_exception dom_implementation_get_feature(
		struct dom_implementation *impl,
		struct dom_string *feature, struct dom_string *version,
		void **object,
		dom_alloc alloc, void *pw)
{
	return impl->get_feature(impl, feature, version, object, alloc, pw);
}
