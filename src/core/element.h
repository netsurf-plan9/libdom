/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_element_h_
#define dom_internal_core_element_h_

#include <stdbool.h>

#include <dom/core/exceptions.h>

struct dom_document;
struct dom_element;
struct dom_namednodemap;
struct dom_node;
struct dom_string;
struct dom_attr;
struct dom_type_info;

/**
 * DOM element node
 */
struct dom_element {
	struct dom_node_internal base;		/**< Base node */

	struct dom_attr *attributes;	/**< Element attributes */

	struct dom_type_info *schema_type_info;	/**< Type information */
};

dom_exception dom_element_create(struct dom_document *doc,
		struct dom_string *name, struct dom_string *namespace,
		struct dom_string *prefix, struct dom_element **result);

dom_exception dom_element_initialise(struct dom_element *el,
		struct dom_string *name, struct dom_string *namespace,
		struct dom_string *prefix, struct dom_element **result);

void dom_element_destroy(struct dom_document *doc,
		struct dom_element *element);

void _dom_element_destroy(struct dom_node_internal *node);

dom_exception dom_element_get_attributes(struct dom_element *element,
		struct dom_namednodemap **result);

dom_exception dom_element_has_attributes(struct dom_element *element,
		bool *result);

struct dom_node *dom_element_get_first_attribute(struct dom_element *element);

/* The virtual functions of dom_element */
dom_exception _dom_element_get_tag_name(struct dom_element *element,
		struct dom_string **name);
dom_exception _dom_element_get_attribute(struct dom_element *element,
		struct dom_string *name, struct dom_string **value);
dom_exception _dom_element_set_attribute(struct dom_element *element,
		struct dom_string *name, struct dom_string *value);
dom_exception _dom_element_remove_attribute(struct dom_element *element, 
		struct dom_string *name);
dom_exception _dom_element_get_attribute_node(struct dom_element *element, 
		struct dom_string *name, struct dom_attr **result);
dom_exception _dom_element_set_attribute_node(struct dom_element *element, 
		struct dom_attr *attr, struct dom_attr **result);
dom_exception _dom_element_remove_attribute_node(struct dom_element *element, 
		struct dom_attr *attr, struct dom_attr **result);
dom_exception _dom_element_get_elements_by_tag_name(struct dom_element *element, 
		struct dom_string *name, struct dom_nodelist **result);
dom_exception _dom_element_get_attribute_ns(struct dom_element *element, 
		struct dom_string *namespace,  struct dom_string *localname, 
		struct dom_string **value);
dom_exception _dom_element_set_attribute_ns(struct dom_element *element,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_string *value);
dom_exception _dom_element_remove_attribute_ns(struct dom_element *element,
		struct dom_string *namespace, struct dom_string *localname);
dom_exception _dom_element_get_attribute_node_ns(struct dom_element *element,
		struct dom_string *namespace, struct dom_string *localname, 
		struct dom_attr **result);
dom_exception _dom_element_set_attribute_node_ns(struct dom_element *element, 
		struct dom_attr *attr, struct dom_attr **result);
dom_exception _dom_element_get_elements_by_tag_name_ns(
		struct dom_element *element, struct dom_string *namespace, 
		struct dom_string *localname, struct dom_nodelist **result);
dom_exception _dom_element_has_attribute(struct dom_element *element,
		struct dom_string *name, bool *result);
dom_exception _dom_element_has_attribute_ns(struct dom_element *element,
		struct dom_string *namespace, struct dom_string *localname, 
		bool *result);
dom_exception _dom_element_get_schema_type_info(struct dom_element *element, 
		struct dom_type_info **result);
dom_exception _dom_element_set_id_attribute(struct dom_element *element, 
		struct dom_string *name, bool is_id);
dom_exception _dom_element_set_id_attribute_ns(struct dom_element *element, 
		struct dom_string *namespace, struct dom_string *localname, 
		bool is_id);
dom_exception _dom_element_set_id_attribute_node(struct dom_element *element,
		struct dom_attr *id_attr, bool is_id);

#define DOM_ELEMENT_VTABLE \
	_dom_element_get_tag_name, \
	_dom_element_get_attribute, \
	_dom_element_set_attribute, \
	_dom_element_remove_attribute, \
	_dom_element_get_attribute_node, \
	_dom_element_set_attribute_node, \
	_dom_element_remove_attribute_node, \
	_dom_element_get_elements_by_tag_name, \
	_dom_element_get_attribute_ns, \
	_dom_element_set_attribute_ns, \
	_dom_element_remove_attribute_ns, \
	_dom_element_get_attribute_node_ns, \
	_dom_element_set_attribute_node_ns, \
	_dom_element_get_elements_by_tag_name_ns, \
	_dom_element_has_attribute, \
	_dom_element_has_attribute_ns, \
	_dom_element_get_schema_type_info, \
	_dom_element_set_id_attribute, \
	_dom_element_set_id_attribute_ns, \
	_dom_element_set_id_attribute_node

#endif
