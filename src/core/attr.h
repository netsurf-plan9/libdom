/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_attr_h_
#define dom_internal_core_attr_h_

#include <dom/core/exceptions.h>

struct dom_document;
struct dom_string;
struct dom_type_info;

dom_exception dom_attr_create(struct dom_document *doc,
		struct dom_string *name, struct dom_string *namespace,
		struct dom_string *prefix, struct dom_attr **result);
void dom_attr_destroy(struct dom_document *doc, struct dom_attr *attr);
/* The virtual destroy function */
void _dom_attr_destroy(dom_node_internal *node);

/* Virtual functions for dom attr */
dom_exception _dom_attr_get_name(struct dom_attr *attr,
                struct dom_string **result);
dom_exception _dom_attr_get_specified(struct dom_attr *attr, bool *result);
dom_exception _dom_attr_get_value(struct dom_attr *attr,
                struct dom_string **result);
dom_exception _dom_attr_set_value(struct dom_attr *attr,
                struct dom_string *value);
dom_exception _dom_attr_get_owner(struct dom_attr *attr,
                struct dom_element **result);
dom_exception _dom_attr_get_schema_type_info(struct dom_attr *attr,
                struct dom_type_info **result);
dom_exception _dom_attr_is_id(struct dom_attr *attr, bool *result);

#define DOM_ATTR_VTABLE 	\
	_dom_attr_get_name, \
	_dom_attr_get_specified, \
	_dom_attr_get_value, \
	_dom_attr_set_value, \
	_dom_attr_get_owner, \
	_dom_attr_get_schema_type_info, \
	_dom_attr_is_id

#endif
