/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_document_type_h_
#define dom_internal_core_document_type_h_

struct dom_document_type;

/* Destroy a document type */
void dom_document_type_destroy(struct dom_node_internal *doctypenode);

/* The virtual functions of DocumentType */
dom_exception _dom_document_type_get_name(struct dom_document_type *doc_type,
		struct dom_string **result);
dom_exception _dom_document_type_get_entities(
		struct dom_document_type *doc_type,
		struct dom_namednodemap **result);
dom_exception _dom_document_type_get_notations(
		struct dom_document_type *doc_type,
		struct dom_namednodemap **result);
dom_exception _dom_document_type_get_public_id(
		struct dom_document_type *doc_type,
		struct dom_string **result);
dom_exception _dom_document_type_get_system_id(
		struct dom_document_type *doc_type,
		struct dom_string **result);
dom_exception _dom_document_type_get_internal_subset(
		struct dom_document_type *doc_type,
		struct dom_string **result);

#define DOM_DOCUMENT_TYPE_VTABLE \
	_dom_document_type_get_name, \
	_dom_document_type_get_entities, \
	_dom_document_type_get_notations, \
	_dom_document_type_get_public_id, \
	_dom_document_type_get_system_id, \
	_dom_document_type_get_internal_subset

#endif

