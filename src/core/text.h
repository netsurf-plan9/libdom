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
struct lwc_context_s;
struct lwc_string_s;

/**
 * A DOM text node
 */
struct dom_text {
	struct dom_characterdata base;	/**< Base node */

	bool element_content_whitespace;	/**< This node is element
						 * content whitespace */
};

/* Constructor and Destructor */
dom_exception _dom_text_create(struct dom_document *doc,
		struct lwc_string_s *name, struct dom_string *value,
		struct dom_text **result);

void _dom_text_destroy(struct dom_document *doc, struct dom_text *text);

dom_exception _dom_text_initialise(struct dom_text *text,
		struct dom_document *doc, dom_node_type type,
		struct lwc_string_s *name, struct dom_string *value);

void _dom_text_finalise(struct dom_document *doc, struct dom_text *text);


/* Virtual functions for dom_text */
dom_exception _dom_text_split_text(struct dom_text *text,
		unsigned long offset, struct dom_text **result);
dom_exception _dom_text_get_is_element_content_whitespace(
		struct dom_text *text, bool *result);
dom_exception _dom_text_get_whole_text(struct dom_text *text,
		struct dom_string **result);
dom_exception _dom_text_replace_whole_text(struct dom_text *text,
		struct dom_string *content, struct dom_text **result);

#define DOM_TEXT_VTABLE \
	_dom_text_split_text, \
	_dom_text_get_is_element_content_whitespace, \
	_dom_text_get_whole_text, \
	_dom_text_replace_whole_text


/* Following comes the protected vtable  */
void __dom_text_destroy(struct dom_node_internal *node);
dom_exception _dom_text_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret);
dom_exception _dom_text_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old);

#define DOM_TEXT_PROTECT_VTABLE \
	__dom_text_destroy, \
	_dom_text_alloc, \
	_dom_text_copy


extern struct dom_text_vtable text_vtable;

#endif
