/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_documentfragment_h_
#define dom_internal_core_documentfragment_h_

#include <dom/core/exceptions.h>

struct dom_document_fragment;
struct dom_document;
struct dom_string;

dom_exception dom_document_fragment_create(struct dom_document *doc,
		struct dom_string *name, struct dom_string *value,
		struct dom_document_fragment **result);

#endif
