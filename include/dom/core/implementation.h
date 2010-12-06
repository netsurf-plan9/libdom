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
#include <dom/events/document_event.h>
#include <dom/functypes.h>

struct dom_document;
struct dom_document_type;

typedef const char *dom_implementation;

dom_exception dom_implementation_has_feature(
		const char *feature, const char *version,
		bool *result);

dom_exception dom_implementation_create_document_type(
		const char *qname,
		const char *public_id, const char *system_id,
		dom_alloc alloc, void *pw,
		struct dom_document_type **doctype);

dom_exception dom_implementation_create_document(
		const char *namespace, const char *qname,
		struct dom_document_type *doctype,
		dom_alloc alloc, void *pw,
		dom_events_default_action_fetcher daf,
		struct dom_document **doc);

dom_exception dom_implementation_get_feature(
		const char *feature, const char *version,
		void **object);

#endif
