/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_text_h_
#define dom_internal_core_text_h_

#include <dom/core/exceptions.h>

#include "core/node.h"

struct dom_document;
struct dom_string;
struct dom_text;

dom_exception dom_text_create(struct dom_document *doc, dom_node_type type,
		struct dom_string *name, struct dom_string *value,
		struct dom_text **result);

#endif
