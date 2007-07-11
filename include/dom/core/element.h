/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_element_h_
#define dom_core_element_h_

#include <stdbool.h>

#include <dom/core/exceptions.h>

struct dom_attr;
struct dom_element;
struct dom_nodelist;
struct dom_string;
struct dom_type_info;

dom_exception dom_element_get_tag_name(struct dom_element *element,
		struct dom_string **name);

dom_exception dom_element_get_attribute(struct dom_element *element,
		struct dom_string *name, struct dom_string **value);
dom_exception dom_element_set_attribute(struct dom_element *element,
		struct dom_string *name, struct dom_string *value);
dom_exception dom_element_remove_attribute(struct dom_element *element,
		struct dom_string *name);

dom_exception dom_element_get_attribute_node(struct dom_element *element,
		struct dom_string *name, struct dom_attr **result);
dom_exception dom_element_set_attribute_node(struct dom_element *element,
		struct dom_attr *attr, struct dom_attr **result);
dom_exception dom_element_remove_attribute_node(struct dom_element *element,
		struct dom_attr *attr, struct dom_attr **result);

dom_exception dom_element_get_elements_by_tag_name(
		struct dom_element *element, struct dom_string *name,
		struct dom_nodelist **result);

dom_exception dom_element_get_attribute_ns(struct dom_element *element,
		struct dom_string *namespace, struct dom_string *localname,
		struct dom_string **value);
dom_exception dom_element_set_attribute_ns(struct dom_element *element,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_string *value);
dom_exception dom_element_remove_attribute_ns(struct dom_element *element,
		struct dom_string *namespace, struct dom_string *localname);

dom_exception dom_element_get_attribute_node_ns(struct dom_element *element,
		struct dom_string *namespace, struct dom_string *localname,
		struct dom_attr **result);
dom_exception dom_element_set_attribute_node_ns(struct dom_element *element,
		struct dom_attr *attr, struct dom_attr **result);

dom_exception dom_element_get_elements_by_tag_name_ns(
		struct dom_element *element, struct dom_string *namespace,
		struct dom_string *localname, struct dom_nodelist **result);

dom_exception dom_element_has_attribute(struct dom_element *element,
		struct dom_string *name, bool *result);
dom_exception dom_element_has_attribute_ns(struct dom_element *element,
		struct dom_string *namespace, struct dom_string *localname,
		bool *result);

dom_exception dom_element_get_schema_type_info(struct dom_element *element,
		struct dom_type_info **result);

dom_exception dom_element_set_id_attribute(struct dom_element *element,
		struct dom_string *name, bool is_id);
dom_exception dom_element_set_id_attribute_ns(struct dom_element *element,
		struct dom_string *namespace, struct dom_string *localname,
		bool is_id);
dom_exception dom_element_set_id_attribute_node(struct dom_element *element,
		struct dom_attr *id_attr, bool is_id);

#endif
