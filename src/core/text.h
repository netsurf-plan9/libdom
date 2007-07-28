/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_text_h_
#define dom_internal_core_text_h_

#include <stdbool.h>

#include <dom/core/exceptions.h>

#include "core/characterdata.h"

struct dom_document;
struct dom_string;

/**
 * A DOM text node
 */
struct dom_text {
	struct dom_characterdata base;	/**< Base node */

	bool element_content_whitespace;	/**< This node is element
						 * content whitespace */
};

dom_exception dom_text_create(struct dom_document *doc,
		struct dom_string *name, struct dom_string *value,
		struct dom_text **result);

void dom_text_destroy(struct dom_document *doc, struct dom_text *text);

dom_exception dom_text_initialise(struct dom_text *text,
		struct dom_document *doc, dom_node_type type,
		struct dom_string *name, struct dom_string *value);

void dom_text_finalise(struct dom_document *doc, struct dom_text *text);

#endif
