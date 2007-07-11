/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_attr_h_
#define dom_core_attr_h_

#include <stdbool.h>

#include <dom/core/exceptions.h>

struct dom_element;
struct dom_type_info;
struct dom_node;
struct dom_attr;
struct dom_string;

dom_exception dom_attr_get_name(struct dom_attr *attr,
		struct dom_string **result);
dom_exception dom_attr_get_specified(struct dom_attr *attr, bool *result);
dom_exception dom_attr_get_value(struct dom_attr *attr,
		struct dom_string **result);
dom_exception dom_attr_set_value(struct dom_attr *attr,
		struct dom_string *value);
dom_exception dom_attr_get_owner(struct dom_attr *attr,
		struct dom_element **result);
dom_exception dom_attr_get_type_info(struct dom_attr *attr,
		struct dom_type_info **result);
dom_exception dom_attr_is_id(struct dom_attr *attr, bool *result);

#endif
