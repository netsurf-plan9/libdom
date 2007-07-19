/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2007 James Shaw <jshaw@netsurf-browser.org>
 */

#include <dom/core/document_type.h>

#include "core/node.h"
#include "utils/utils.h"

/**
 * DOM DocumentType node
 */
struct dom_document_type {
	struct dom_node base;		/**< Base node */

	/** \todo other members */
};

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
dom_exception dom_document_type_get_name(struct dom_document_type *doc_type,
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
dom_exception dom_document_type_get_entities(
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
dom_exception dom_document_type_get_notations(
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
dom_exception dom_document_type_get_public_id(
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
dom_exception dom_document_type_get_system_id(
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
dom_exception dom_document_type_get_internal_subset(
		struct dom_document_type *doc_type,
		struct dom_string **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

