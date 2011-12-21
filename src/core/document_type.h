/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_document_type_h_
#define dom_internal_core_document_type_h_

#include <dom/core/document_type.h>

struct dom_namednodemap;

/* Create a DOM document type */
dom_exception _dom_document_type_create(dom_string *qname,
		dom_string *public_id, 
		dom_string *system_id,
		dom_document_type **doctype);
/* Destroy a document type */
void _dom_document_type_destroy(dom_node_internal *doctypenode);
dom_exception _dom_document_type_initialise(dom_document_type *doctype,
		dom_string *qname, dom_string *public_id,
		dom_string *system_id);
void _dom_document_type_finalise(dom_document_type *doctype);

/* The virtual functions of DocumentType */
dom_exception _dom_document_type_get_name(dom_document_type *doc_type,
		dom_string **result);
dom_exception _dom_document_type_get_entities(
		dom_document_type *doc_type,
		struct dom_namednodemap **result);
dom_exception _dom_document_type_get_notations(
		dom_document_type *doc_type,
		struct dom_namednodemap **result);
dom_exception _dom_document_type_get_public_id(
		dom_document_type *doc_type,
		dom_string **result);
dom_exception _dom_document_type_get_system_id(
		dom_document_type *doc_type,
		dom_string **result);
dom_exception _dom_document_type_get_internal_subset(
		dom_document_type *doc_type,
		dom_string **result);

#define DOM_DOCUMENT_TYPE_VTABLE \
	_dom_document_type_get_name, \
	_dom_document_type_get_entities, \
	_dom_document_type_get_notations, \
	_dom_document_type_get_public_id, \
	_dom_document_type_get_system_id, \
	_dom_document_type_get_internal_subset

/* Following comes the protected vtable  */
void _dom_dt_destroy(dom_node_internal *node);
dom_exception _dom_dt_copy(dom_node_internal *old, dom_node_internal **copy);

#define DOM_DT_PROTECT_VTABLE \
	_dom_dt_destroy, \
	_dom_dt_copy

#endif
