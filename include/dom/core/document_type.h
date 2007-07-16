/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_document_type_h_
#define dom_core_document_type_h_

#include <stdbool.h>

#include <dom/core/exceptions.h>

struct dom_string;
struct dom_document_type;

dom_exception dom_document_type_get_name(struct dom_document_type *docType,
		struct dom_string **result);

#endif
