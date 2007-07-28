/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_entityreference_h_
#define dom_internal_core_entityreference_h_

#include <dom/core/exceptions.h>

struct dom_document;
struct dom_entity_reference;
struct dom_string;

dom_exception dom_entity_reference_create(struct dom_document *doc,
		struct dom_string *name, struct dom_string *value,
		struct dom_entity_reference **result);

#endif
