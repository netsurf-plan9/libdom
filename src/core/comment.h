/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_comment_h_
#define dom_internal_core_comment_h_

#include <dom/core/exceptions.h>
#include <dom/core/comment.h>

struct dom_comment;
struct dom_document;
struct dom_string;
struct lwc_string_s;

dom_exception _dom_comment_create(struct dom_document *doc,
		struct lwc_string_s *name, struct dom_string *value,
		struct dom_comment **result);

#define  _dom_comment_initialise _dom_characterdata_initialise
#define  _dom_comment_finalise _dom_characterdata_finalise

void _dom_comment_destroy(struct dom_document *doc,
		struct dom_comment *comment);

/* Following comes the protected vtable  */
void __dom_comment_destroy(struct dom_node_internal *node);
dom_exception _dom_comment_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret);
dom_exception _dom_comment_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old);

#define DOM_COMMENT_PROTECT_VTABLE \
	__dom_comment_destroy, \
	_dom_comment_alloc, \
	_dom_comment_copy

#endif
