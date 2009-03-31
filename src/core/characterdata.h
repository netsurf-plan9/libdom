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

dom_characterdata *dom_characterdata_create(struct dom_document *doc);
dom_exception dom_characterdata_initialise(struct dom_characterdata *cdata,
		struct dom_document *doc, dom_node_type type,
		struct dom_string *name, struct dom_string *value);

void dom_characterdata_finalise(struct dom_document *doc,
		struct dom_characterdata *cdata);

/* The virtual functions for characterdata */
dom_exception _dom_characterdata_get_data(struct dom_characterdata *cdata,
		struct dom_string **data);
dom_exception _dom_characterdata_set_data(struct dom_characterdata *cdata,
		struct dom_string *data);
dom_exception _dom_characterdata_get_length(struct dom_characterdata *cdata,
		unsigned long *length);
dom_exception _dom_characterdata_substring_data(
		struct dom_characterdata *cdata, unsigned long offset,
		unsigned long count, struct dom_string **data);
dom_exception _dom_characterdata_append_data(struct dom_characterdata *cdata,
		struct dom_string *data);
dom_exception _dom_characterdata_insert_data(struct dom_characterdata *cdata,
		unsigned long offset, struct dom_string *data);
dom_exception _dom_characterdata_delete_data(struct dom_characterdata *cdata,
		unsigned long offset, unsigned long count);
dom_exception _dom_characterdata_replace_data(struct dom_characterdata *cdata,
		unsigned long offset, unsigned long count,
		struct dom_string *data);

#define DOM_CHARACTERDATA_VTABLE \
	_dom_characterdata_get_data, \
	_dom_characterdata_set_data, \
	_dom_characterdata_get_length, \
	_dom_characterdata_substring_data, \
	_dom_characterdata_append_data, \
	_dom_characterdata_insert_data, \
	_dom_characterdata_delete_data, \
	_dom_characterdata_replace_data 

#endif
