/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_characterdata_h_
#define dom_internal_core_characterdata_h_

#include <dom/core/characterdata.h>

#include "core/node.h"

/**
 * DOM character data node
 */
struct dom_characterdata {
	struct dom_node_internal base;		/**< Base node */
};

/* The CharacterData is a intermediate node type, so the following function
 * may never be used */
dom_characterdata *_dom_characterdata_create(void);
dom_exception _dom_characterdata_initialise(struct dom_characterdata *cdata,
		struct dom_document *doc, dom_node_type type,
		dom_string *name, dom_string *value);

void _dom_characterdata_finalise(struct dom_characterdata *cdata);

/* The virtual functions for dom_characterdata */
dom_exception _dom_characterdata_get_data(struct dom_characterdata *cdata,
		dom_string **data);
dom_exception _dom_characterdata_set_data(struct dom_characterdata *cdata,
		dom_string *data);
dom_exception _dom_characterdata_get_length(struct dom_characterdata *cdata,
		unsigned long *length);
dom_exception _dom_characterdata_substring_data(
		struct dom_characterdata *cdata, unsigned long offset,
		unsigned long count, dom_string **data);
dom_exception _dom_characterdata_append_data(struct dom_characterdata *cdata,
		dom_string *data);
dom_exception _dom_characterdata_insert_data(struct dom_characterdata *cdata,
		unsigned long offset, dom_string *data);
dom_exception _dom_characterdata_delete_data(struct dom_characterdata *cdata,
		unsigned long offset, unsigned long count);
dom_exception _dom_characterdata_replace_data(struct dom_characterdata *cdata,
		unsigned long offset, unsigned long count,
		dom_string *data);

#define DOM_CHARACTERDATA_VTABLE \
	_dom_characterdata_get_data, \
	_dom_characterdata_set_data, \
	_dom_characterdata_get_length, \
	_dom_characterdata_substring_data, \
	_dom_characterdata_append_data, \
	_dom_characterdata_insert_data, \
	_dom_characterdata_delete_data, \
	_dom_characterdata_replace_data 

/* Following comes the protected vtable 
 *
 * Only the _copy function can be used by sub-class of this.
 */
void _dom_characterdata_destroy(dom_node_internal *node);
dom_exception _dom_characterdata_copy(dom_node_internal *old, 
		dom_node_internal **copy);

#define DOM_CHARACTERDATA_PROTECT_VTABLE \
	_dom_characterdata_destroy, \
	_dom_characterdata_copy

extern struct dom_characterdata_vtable characterdata_vtable;

dom_exception _dom_characterdata_copy_internal(dom_characterdata *old, 
		dom_characterdata *new);
#define dom_characterdata_copy_internal(o, n) \
		_dom_characterdata_copy_internal( \
		(dom_characterdata *) (o), (dom_characterdata *) (n))

#endif
