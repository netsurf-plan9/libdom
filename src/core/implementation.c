/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/core/implementation.h>

#include "core/document.h"
#include "core/document_type.h"

#include "utils/namespace.h"
#include "utils/utils.h"
#include "utils/validate.h"

/**
 * Test whether a DOM implementation implements a specific feature
 * and version
 *
 * \param feature  The feature to test for
 * \param version  The version number of the feature to test for
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_implementation_has_feature(
		struct dom_string *feature, struct dom_string *version,
		bool *result)
{
	UNUSED(feature);
	UNUSED(version);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Create a document type node
 *
 * \param qname      The qualified name of the document type
 * \param public_id  The external subset public identifier
 * \param system_id  The external subset system identifier
 * \param doctype    Pointer to location to receive result
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
		struct dom_string *qname, struct dom_string *public_id, 
		struct dom_string *system_id,
		dom_alloc alloc, void *pw,
		struct dom_document_type **doctype)
{
	struct dom_document_type *d;
	struct dom_string *prefix = NULL, *lname = NULL;
	dom_exception err;

	if (qname != NULL && _dom_validate_name(qname) == false)
		return DOM_INVALID_CHARACTER_ERR;

	err = _dom_namespace_split_qname(qname, &prefix, &lname);
	if (err != DOM_NO_ERR)
		return err;

	if ((prefix != NULL && _dom_validate_ncname(prefix) == false) ||
			(lname != NULL && _dom_validate_ncname(lname) == false))
		return DOM_NAMESPACE_ERR;

	/* Create the doctype */
	err = _dom_document_type_create(qname, public_id, system_id,
			alloc, pw, &d);
	if (err != DOM_NO_ERR)
		return err;

	*doctype = d;
	if (prefix != NULL)
		dom_string_unref(prefix);
	if (lname != NULL)
		dom_string_unref(lname);

	return DOM_NO_ERR;
}

/**
 * Create a document node
 *
 * \param namespace  The namespace URI of the document element
 * \param qname      The qualified name of the document element
 * \param doctype    The type of document to create
 * \param doc        Pointer to location to receive result
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
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_document_type *doctype,
		dom_alloc alloc, void *pw,
		dom_events_default_action_fetcher daf,
		struct dom_document **doc)
{
	struct dom_document *d;
	dom_exception err;

	if (qname != NULL && _dom_validate_name(qname) == false)
		return  DOM_INVALID_CHARACTER_ERR;
  
	err = _dom_namespace_validate_qname(qname, namespace);
	if (err != DOM_NO_ERR)
		return DOM_NAMESPACE_ERR;

	if (doctype != NULL && dom_node_get_parent(doctype) != NULL)
		return DOM_WRONG_DOCUMENT_ERR;

	/* Create document object */
	err = _dom_document_create(alloc, pw, daf, &d);
	if (err != DOM_NO_ERR)
		return err;

	/* Set its doctype, if necessary */
	if (doctype != NULL) {
		struct dom_node *ins_doctype = NULL;

		err = dom_node_append_child((struct dom_node *) d, 
				(struct dom_node *) doctype, &ins_doctype);
		if (err != DOM_NO_ERR) {
			dom_node_unref((struct dom_node *) d);
			return err;
		}

		/* Not interested in inserted doctype */
		if (ins_doctype != NULL)
			dom_node_unref(ins_doctype);
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
 * \param feature  The requested feature
 * \param version  The version number of the feature
 * \param object   Pointer to location to receive object
 * \return DOM_NO_ERR.
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function.
 */
dom_exception dom_implementation_get_feature(
		struct dom_string *feature, struct dom_string *version,
		void **object)
{
	UNUSED(feature);
	UNUSED(version);
	UNUSED(object);

	return DOM_NOT_SUPPORTED_ERR;
}
