/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

/** \file
 * This file defines all the grubby details that implementation backends
 * need to know in order to permit themselves to be bootstrapped.
 *
 * The DOMImplementation and DOMImplementationList implementations also
 * include this, as those types are defined here.
 *
 * The Document implementation includes this as it needs the declaration of
 * dom_document_set_doctype.
 *
 * No other client should be including this.
 */

#ifndef dom_bootstrap_implpriv_h_
#define dom_bootstrap_implpriv_h_

#include <inttypes.h>
#include <stdbool.h>

#include <dom/core/exceptions.h>
#include <dom/functypes.h>

struct dom_document;
struct dom_document_type;
struct dom_string;

/**
 * DOM Implementation
 */
struct dom_implementation {
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
	dom_exception (*has_feature)(struct dom_implementation *impl,
			struct dom_string *feature,
			struct dom_string *version,
			bool *result);

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
	dom_exception (*create_document_type)(
			struct dom_implementation *impl,
			struct dom_string *qname,
			struct dom_string *public_id,
			struct dom_string *system_id,
			struct dom_document_type **doctype,
			dom_alloc alloc, void *pw);

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
	 * The doctype will be referenced, so the client need not do this
	 * explicitly. The client must unref the doctype once it has
	 * finished with it.
	 */
	dom_exception (*create_document)(struct dom_implementation *impl,
			struct dom_string *namespace,
			struct dom_string *qname,
			struct dom_document_type *doctype,
			struct dom_document **doc,
			dom_alloc alloc, void *pw);

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
	dom_exception (*get_feature)(struct dom_implementation *impl,
			struct dom_string *feature,
			struct dom_string *version,
			void **object,
			dom_alloc alloc, void *pw);

	/**
	 * Destroy a DOM implementation instance
	 *
	 * \param impl  The instance to destroy
	 */
	void (*destroy)(struct dom_implementation *impl);

	uint32_t refcnt;		/**< Reference count */
};


/**
 * An item in a DOM Implementation List
 */
struct dom_implementation_list_item {
	struct dom_implementation *impl;	/**< Implementation */

	struct dom_implementation_list_item *next;	/**< Next in list */
	struct dom_implementation_list_item *prev;	/**< Prev in list */
};

/**
 * DOM Implementation List
 */
struct dom_implementation_list {
	struct dom_implementation_list_item *head;	/**< Head of list */

	dom_alloc alloc;		/**< Memory (de)allocation function */
	void *pw;			/**< Pointer to client data */

	uint32_t refcnt;		/**< Reference count */
};


/**
 * DOM Implementation Source
 *
 * This is simply a pair of function pointers in a struct.
 *
 * This is assumed to be statically allocated within the backend.
 */
struct dom_implementation_source {
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
	dom_exception (*get_dom_implementation)(
			struct dom_string *features,
			struct dom_implementation **impl,
			dom_alloc alloc, void *pw);

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
	dom_exception (*get_dom_implementation_list)(
			struct dom_string *features,
			struct dom_implementation_list **list,
			dom_alloc alloc, void *pw);
};

/* Register a source with the DOM library */
dom_exception dom_register_source(struct dom_implementation_source *source,
		dom_alloc alloc, void *pw);

/* Set a Document's DocumentType */
dom_exception dom_document_set_doctype(struct dom_document *doc,
		struct dom_document_type *doctype);

#endif
