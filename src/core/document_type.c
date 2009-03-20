/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2007 James Shaw <jshaw@netsurf-browser.org>
 */

#include <dom/core/document_type.h>
#include <dom/core/string.h>
#include <dom/bootstrap/implpriv.h>

#include "core/document_type.h"
#include "core/node.h"
#include "utils/utils.h"

/**
 * DOM DocumentType node
 */
struct dom_document_type {
	struct dom_node_internal base;		/**< Base node */

	/** \todo other members */
	struct dom_string *public_id;	/**< Doctype public ID */
	struct dom_string *system_id;	/**< Doctype system ID */

	dom_alloc alloc;		/**< Memory (de)allocation function */
	void *pw;			/**< Pointer to private data */
};

static struct dom_document_type_vtable document_type_vtable = {
	{
		DOM_NODE_VTABLE	
	},
	DOM_DOCUMENT_TYPE_VTABLE
};

/**
 * Create a document type node
 *
 * \param qname      The qualified name of the document type
 * \param public_id  The external subset public identifier
 * \param system_id  The external subset system identifier
 * \param alloc      Memory (de)allocation function
 * \param pw         Pointer to client-specific private data
 * \param doctype    Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion.
 *
 * The doctype will be referenced, so the client need not do so
 * explicitly. The client must unref the doctype once it has
 * finished with it.
 */
dom_exception dom_document_type_create(struct dom_string *qname,
		struct dom_string *public_id, struct dom_string *system_id,
		dom_alloc alloc, void *pw, struct dom_document_type **doctype)
{
	struct dom_document_type *result;
	dom_exception err;

	/* Create node */
	result = alloc(NULL, sizeof(struct dom_document_type), pw);
	if (result == NULL)
		return DOM_NO_MEM_ERR;

	/* Initialise base node */
	err = dom_node_initialise(&result->base, NULL, DOM_DOCUMENT_TYPE_NODE,
			qname, NULL, NULL, NULL);
	if (err != DOM_NO_ERR) {
		alloc(result, 0, pw);
		return err;
	}

	/* Initialise the vtable */
	result->base.base.vtable = &document_type_vtable;
	result->base.destroy = &dom_document_type_destroy;

	/* Get public and system IDs */
	dom_string_ref(public_id);
	result->public_id = public_id;

	dom_string_ref(system_id);
	result->system_id = system_id;

	/* Fill in allocation information */
	result->alloc = alloc;
	result->pw = pw;

	*doctype = result;

	return DOM_NO_ERR;
}

/**
 * Destroy a DocumentType node
 *
 * \param doctype  The DocumentType node to destroy
 *
 * The contents of ::doctype will be destroyed and ::doctype will be freed.
 */
void dom_document_type_destroy(struct dom_node_internal *doctypenode)
{
	struct dom_document_type *doctype = 
			(struct dom_document_type *)doctypenode;

	/* Finish with public and system IDs */
	dom_string_unref(doctype->system_id);
	dom_string_unref(doctype->public_id);

	/* Finalise base class */
	dom_node_finalise(doctype->base.owner, &doctype->base);

	/* Free doctype */
	doctype->alloc(doctype, 0, doctype->pw);
}

/**
 * Retrieve a document type's name
 *
 * \param doc_type  Document type to retrieve name from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_type_get_name(struct dom_document_type *doc_type,
		struct dom_string **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve a document type's entities
 *
 * \param doc_type  Document type to retrieve entities from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned map will have its reference count increased. It is
 * the responsibility of the caller to unref the map once it has
 * finished with it.
 */
dom_exception _dom_document_type_get_entities(
		struct dom_document_type *doc_type,
		struct dom_namednodemap **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve a document type's notations
 *
 * \param doc_type  Document type to retrieve notations from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned map will have its reference count increased. It is
 * the responsibility of the caller to unref the map once it has
 * finished with it.
 */
dom_exception _dom_document_type_get_notations(
		struct dom_document_type *doc_type,
		struct dom_namednodemap **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve a document type's public id
 *
 * \param doc_type  Document type to retrieve public id from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_type_get_public_id(
		struct dom_document_type *doc_type,
		struct dom_string **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve a document type's system id
 *
 * \param doc_type  Document type to retrieve system id from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_type_get_system_id(
		struct dom_document_type *doc_type,
		struct dom_string **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve a document type's internal subset
 *
 * \param doc_type  Document type to retrieve internal subset from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception _dom_document_type_get_internal_subset(
		struct dom_document_type *doc_type,
		struct dom_string **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

