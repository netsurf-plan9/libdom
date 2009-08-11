/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

/**
 * Note: The DOMImplementation Object here is a singleton object. It is 
 * initialised when the libDOM is initialised, it registers itself into
 * the implreg and clients of it can get it by calling:
 *
 * dom_implregistry_get_dom_implementation or 
 * dom_implregistry_get_dom_implementation_list
 * 
 */

#include <dom/bootstrap/implpriv.h>
#include <dom/bootstrap/implregistry.h>
#include <dom/dom.h>

#include <libwapcaplet/libwapcaplet.h>

#include "core/node.h"
#include "core/document_type.h"

#include "utils/utils.h"
#include "utils/validate.h"
#include "utils/namespace.h"

#include "bootstrap/implementation.h"

static dom_alloc _alloc;
static void *_pw;

static dom_exception impl_get_dom_implementation(
		struct dom_string *features,
		struct dom_implementation **impl);
static dom_exception impl_get_dom_implementation_list(
		struct dom_string *features,
		struct dom_implementation_list **list);

static dom_exception impl_implementation_has_feature(
		struct dom_implementation *impl,
		struct dom_string *feature,
		struct dom_string *version,
		bool *result);
static dom_exception impl_implementation_create_document_type(
		struct dom_implementation *impl,
		struct dom_string *qname,
		struct dom_string *public_id,
		struct dom_string *system_id,
		dom_alloc alloc, void *pw, struct lwc_context_s *ctx,
		struct dom_document_type **doctype);
static dom_exception impl_implementation_create_document(
		struct dom_implementation *impl,
		struct dom_string *namespace,
		struct dom_string *qname,
		struct dom_document_type *doctype,
		dom_alloc alloc, void *pw, struct lwc_context_s *ctx,
		struct dom_document **doc);
static dom_exception impl_implementation_get_feature(
		struct dom_implementation *impl,
		struct dom_string *feature,
		struct dom_string *version,
		void **object);
static void dom_implementation_destroy(struct dom_implementation *impl);


static struct dom_implementation_source dom_impl_src = {
	impl_get_dom_implementation,
	impl_get_dom_implementation_list
};

static struct dom_implementation dom_impl = {
	impl_implementation_has_feature,
	impl_implementation_create_document_type,
	impl_implementation_create_document,
	impl_implementation_get_feature,
	dom_implementation_destroy,
	0
};

/**
 * Get a DOM implementation that supports the requested features
 *
 * \param features  String containing required features
 * \param impl      Pointer to location to receive implementation
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
dom_exception impl_get_dom_implementation(
		struct dom_string *features,
		struct dom_implementation **impl)
{
	UNUSED(features);

	dom_impl.refcnt++;

	*impl = &dom_impl;

	return DOM_NO_ERR;
}

/**
 * Get a list of DOM implementations that support the requested
 * features
 *
 * \param features  String containing required features
 * \param list      Pointer to location to receive list
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
dom_exception impl_get_dom_implementation_list(
		struct dom_string *features,
		struct dom_implementation_list **list)
{
	struct dom_implementation_list *l;
	struct dom_implementation_list_item *i;

	UNUSED(features);

	l = _alloc(NULL, sizeof(struct dom_implementation_list), _pw);
	if (l == NULL)
		return DOM_NO_MEM_ERR;

	i = _alloc(NULL, sizeof(struct dom_implementation_list_item), _pw);
	if (i == NULL) {
		_alloc(l, 0, _pw);
		return DOM_NO_MEM_ERR;
	}

	i->impl = &dom_impl;
	i->next = NULL;
	i->prev = NULL;

	l->head = i;

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
dom_exception impl_implementation_has_feature(
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
 * \return DOM_NO_ERR on success,
 *         DOM_INVALID_CHARACTER_ERR if ::qname is invalid,
 *         DOM_NAMESPACE_ERR         if ::qname is malformed.
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function.
 *
 * The doctype will be referenced, so the client need not do this
 * explicitly. The client must unref the doctype once it has
 * finished with it.
 */
dom_exception impl_implementation_create_document_type(
		struct dom_implementation *impl,
		struct dom_string *qname,
		struct dom_string *public_id,
		struct dom_string *system_id,
		dom_alloc alloc, void *pw, struct lwc_context_s *ctx,
		struct dom_document_type **doctype)
{
	struct dom_document_type *d;
	struct dom_string *prefix = NULL, *lname = NULL;
	dom_exception err;

	UNUSED(impl);

	if (qname != NULL && _dom_validate_name(qname) == false)
		return DOM_INVALID_CHARACTER_ERR;

	err = _dom_namespace_split_qname(qname, &prefix, &lname);
	if (err != DOM_NO_ERR)
		return err;

	if ((prefix != NULL && _dom_validate_ncname(prefix) == false) ||
			(lname != NULL && _dom_validate_ncname(lname) == false))
		return DOM_NAMESPACE_ERR;

	/* Create the doctype */
	err = dom_document_type_create(qname, public_id, system_id,
			alloc, pw, ctx, &d);
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
 * \param impl       The implementation to create the node
 * \param namespace  The namespace URI of the document element
 * \param qname      The qualified name of the document element
 * \param doctype    The type of document to create
 * \param doc        Pointer to location to receive result
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
dom_exception impl_implementation_create_document(
		struct dom_implementation *impl,
		struct dom_string *namespace,
		struct dom_string *qname,
		struct dom_document_type *doctype,
		dom_alloc alloc, void *pw, struct lwc_context_s *ctx,
		struct dom_document **doc)
{
	struct dom_document *d;
	dom_exception err;

	if (qname != NULL && _dom_validate_name(qname) == false)
		return  DOM_INVALID_CHARACTER_ERR;
  
	err = _dom_namespace_validate_qname(qname, namespace);
	if (err != DOM_NO_ERR)
		return DOM_NAMESPACE_ERR;

	if (doctype != NULL) {
			if (dom_node_get_parent(doctype) != NULL || 
					_dom_document_type_get_impl(doctype) !=
							impl)
				return DOM_WRONG_DOCUMENT_ERR;
	}

	/* Create document object */
	err = dom_document_create(impl, alloc, pw, ctx, &d);
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
 * \param impl     The implementation to create the object
 * \param feature  The requested feature
 * \param version  The version number of the feature
 * \param object   Pointer to location to receive object
 * \return DOM_NO_ERR.
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function.
 */
dom_exception impl_implementation_get_feature(
		struct dom_implementation *impl,
		struct dom_string *feature,
		struct dom_string *version,
		void **object)
{
	UNUSED(impl);
	UNUSED(feature);
	UNUSED(version);
	UNUSED(object);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Destroy a DOM implementation instance
 *
 * \param impl  The instance to destroy
 */
void dom_implementation_destroy(struct dom_implementation *impl)
{
	UNUSED(impl);

	/* Nothing to do -- we're statically allocated */
}

/**
 * Initialise the DOM implementation
 *
 * \param alloc  Pointer to memory (de)allocation function
 * \param pw     Pointer to client-specific private data
 * \return DOM_NO_ERR on success
 */
dom_exception _dom_implementation_initialise(dom_alloc alloc, void *pw)
{
	_alloc = alloc;
	_pw = pw;

	return dom_register_source(&dom_impl_src);
}

/**
 * Finalise the DOM implementation
 */
void _dom_implementation_finalise(void)
{
	_alloc = NULL;
	_pw = NULL;
}


