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
#include <dom/core/node.h>

struct dom_element;
struct dom_type_info;
struct dom_node;
struct dom_attr;
struct dom_string;

typedef struct dom_attr dom_attr;

/* DOM Attr vtable */
typedef struct dom_attr_vtable {
	struct dom_node_vtable base;

	dom_exception (*dom_attr_get_name)(struct dom_attr *attr,
			struct dom_string **result);
	dom_exception (*dom_attr_get_specified)(struct dom_attr *attr, bool *result);
	dom_exception (*dom_attr_get_value)(struct dom_attr *attr,
			struct dom_string **result);
	dom_exception (*dom_attr_set_value)(struct dom_attr *attr,
			struct dom_string *value);
	dom_exception (*dom_attr_get_owner)(struct dom_attr *attr,
			struct dom_element **result);
	dom_exception (*dom_attr_get_schema_type_info)(struct dom_attr *attr,
			struct dom_type_info **result);
	dom_exception (*dom_attr_is_id)(struct dom_attr *attr, bool *result);
} dom_attr_vtable;

static inline dom_exception dom_attr_get_name(struct dom_attr *attr,
		struct dom_string **result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_get_name(attr, result);
}
#define dom_attr_get_name(a, r) dom_attr_get_name((struct dom_attr *) (a), \
		(struct dom_string **) (r))

static inline dom_exception dom_attr_get_specified(struct dom_attr *attr, bool *result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_get_specified(attr, result);
}
#define dom_attr_get_specified(a, r) dom_attr_get_specified( \
		(struct dom_attr *) (a), (bool *) (r))

static inline dom_exception dom_attr_get_value(struct dom_attr *attr,
		struct dom_string **result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_get_value(attr, result);
}
#define dom_attr_get_value(a, r) dom_attr_get_value((struct dom_attr *) (a), \
		(struct dom_string **) (r))

static inline dom_exception dom_attr_set_value(struct dom_attr *attr,
		struct dom_string *value)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_set_value(attr, value);
}
#define dom_attr_set_value(a, v) dom_attr_set_value((struct dom_attr *) (a), \
		(struct dom_string *) (v))

static inline dom_exception dom_attr_get_owner(struct dom_attr *attr,
		struct dom_element **result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_get_owner(attr, result);
}
#define dom_attr_get_owner(a, r) dom_attr_get_owner((struct dom_attr *) (a), \
		(struct dom_element **) (r))

static inline dom_exception dom_attr_get_schema_type_info(struct dom_attr *attr,
		struct dom_type_info **result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_get_schema_type_info(attr, result);
}
#define dom_attr_get_schema_type_info(a, r) dom_attr_get_schema_type_info( \
		(struct dom_attr *) (a), (struct dom_type_info **) (r))

static inline dom_exception dom_attr_is_id(struct dom_attr *attr, bool *result)
{
	return ((dom_attr_vtable *) ((dom_node *) attr)->vtable)->
			dom_attr_is_id(attr, result);
}
#define dom_attr_is_id(a, r) dom_attr_is_id((struct dom_attr *) (a), \
		(bool *) (r))

#endif
