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
struct lwc_string_s;

dom_exception _dom_document_fragment_create(struct dom_document *doc,
		struct lwc_string_s *name, struct dom_string *value,
		struct dom_document_fragment **result);

void _dom_document_fragment_destroy(struct dom_document *doc,
		struct dom_document_fragment *frag);

#define _dom_document_fragment_initialise	_dom_node_initialise
#define _dom_document_fragment_finalise		_dom_node_finalise


/* Following comes the protected vtable */
void _dom_df_destroy(struct dom_node_internal *node);
dom_exception _dom_df_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret);
dom_exception _dom_df_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old);

#define DOM_DF_PROTECT_VTABLE \
	_dom_df_destroy, \
	_dom_df_alloc, \
	_dom_df_copy

#endif
