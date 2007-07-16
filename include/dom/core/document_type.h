/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2007 James Shaw <EMAIL_ADDRESS>
 */

#ifndef dom_core_document_type_h_
#define dom_core_document_type_h_

#include <dom/core/exceptions.h>

struct dom_document_type;
struct dom_namednodemap;
struct dom_string;

dom_exception dom_document_type_get_name(struct dom_document_type *doc_type,
		struct dom_string **result);
dom_exception dom_document_type_get_entities(
		struct dom_document_type *doc_type,
		struct dom_namednodemap **result);
dom_exception dom_document_type_get_notations(
		struct dom_document_type *doc_type,
		struct dom_namednodemap **result);
dom_exception dom_document_type_get_public_id(
		struct dom_document_type *doc_type,
		struct dom_string **result);
dom_exception dom_document_type_get_system_id(
		struct dom_document_type *doc_type,
		struct dom_string **result);
dom_exception dom_document_type_get_internal_subset(
		struct dom_document_type *doc_type,
		struct dom_string **result);

#endif
