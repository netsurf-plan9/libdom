/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_document_type_h_
#define dom_internal_core_document_type_h_

struct dom_document_type;
struct dom_resource_mgr;

/* Create a DOM document type */
dom_exception _dom_document_type_create(struct dom_string *qname,
		struct dom_string *public_id, 
		struct dom_string *system_id,
		dom_alloc alloc, void *pw,
		struct dom_document_type **doctype);
/* Destroy a document type */
void _dom_document_type_destroy(struct dom_node_internal *doctypenode);
dom_exception _dom_document_type_initialise(struct dom_document_type *doctype,
		struct dom_string *qname, struct dom_string *public_id,
		struct dom_string *system_id, dom_alloc alloc, void *pw);
void _dom_document_type_finalise(struct dom_document_type *doctype);

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

/* Following comes the protected vtable  */
void _dom_dt_destroy(struct dom_node_internal *node);
dom_exception _dom_dt_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret);
dom_exception _dom_dt_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old);

#define DOM_DT_PROTECT_VTABLE \
	_dom_dt_destroy, \
	_dom_dt_alloc, \
	_dom_dt_copy

/* Helper functions */
void _dom_document_type_get_resource_mgr(
		struct dom_document_type *dt, struct dom_resource_mgr *rm);

#endif
