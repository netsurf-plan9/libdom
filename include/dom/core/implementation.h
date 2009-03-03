/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_implementation_h_
#define dom_core_implementation_h_

#include <stdbool.h>

#include <dom/core/exceptions.h>
#include <dom/functypes.h>
#include <dom/core/string.h>

struct dom_document;
struct dom_document_type;
struct dom_implementation;

void dom_implementation_ref(struct dom_implementation *impl);
void dom_implementation_unref(struct dom_implementation *impl);

dom_exception dom_implementation_has_feature(
		struct dom_implementation *impl,
		struct dom_string *feature, struct dom_string *version,
		bool *result);

dom_exception dom_implementation_create_document_type(
		struct dom_implementation *impl, struct dom_string *qname,
		struct dom_string *public_id, struct dom_string *system_id,
		struct dom_document_type **doctype,
		dom_alloc alloc, void *pw);

dom_exception dom_implementation_create_document(
		struct dom_implementation *impl,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_document_type *doctype,
		struct dom_document **doc,
		dom_alloc alloc, void *pw);

dom_exception dom_implementation_get_feature(
		struct dom_implementation *impl,
		struct dom_string *feature, struct dom_string *version,
		void **object,
		dom_alloc alloc, void *pw);

#endif
