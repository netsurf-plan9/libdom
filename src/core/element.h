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
struct dom_hash_table;

/**
 * DOM element node
 */
struct dom_element {
	struct dom_node_internal base;		/**< Base node */

	struct dom_hash_table *attributes;	/**< Element attributes */

	struct dom_hash_table *ns_attributes;
			/**< Attributes with prefix */

	struct lwc_string_s *id_ns;	/**< The id attribute's namespace */

	struct lwc_string_s *id_name; 	/**< The id attribute's name */

	struct dom_type_info *schema_type_info;	/**< Type information */
};

dom_exception _dom_element_create(struct dom_document *doc,
		struct lwc_string_s *name, struct lwc_string_s *namespace,
		struct lwc_string_s *prefix, struct dom_element **result);

dom_exception _dom_element_initialise(struct dom_document *doc,
		struct dom_element *el, struct lwc_string_s *name, 
		struct lwc_string_s *namespace, struct lwc_string_s *prefix);

void _dom_element_destroy(struct dom_document *doc,
		struct dom_element *element);


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
dom_exception _dom_element_get_elements_by_tag_name(
		struct dom_element *element, struct dom_string *name,
		struct dom_nodelist **result);
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

/* Overloading dom_node functions */
dom_exception _dom_element_get_attributes(dom_node_internal *node,
		struct dom_namednodemap **result);
dom_exception _dom_element_has_attributes(dom_node_internal *node,
		bool *result);
dom_exception _dom_element_normalize(dom_node_internal *node);
dom_exception _dom_element_lookup_prefix(dom_node_internal *node,
		struct dom_string *namespace, struct dom_string **result);
dom_exception _dom_element_is_default_namespace(dom_node_internal *node,
		struct dom_string *namespace, bool *result);
dom_exception _dom_element_lookup_namespace(dom_node_internal *node,
		struct dom_string *prefix, struct dom_string **result);
#define DOM_NODE_VTABLE_ELEMENT \
	_dom_node_get_node_name, \
	_dom_node_get_node_value, \
	_dom_node_set_node_value, \
	_dom_node_get_node_type, \
	_dom_node_get_parent_node, \
	_dom_node_get_child_nodes, \
	_dom_node_get_first_child, \
	_dom_node_get_last_child, \
	_dom_node_get_previous_sibling, \
	_dom_node_get_next_sibling, \
	_dom_element_get_attributes, /*overload*/\
	_dom_node_get_owner_document, \
	_dom_node_insert_before, \
	_dom_node_replace_child, \
	_dom_node_remove_child, \
	_dom_node_append_child, \
	_dom_node_has_child_nodes, \
	_dom_node_clone_node, \
	_dom_node_normalize, \
	_dom_node_is_supported, \
	_dom_node_get_namespace, \
	_dom_node_get_prefix, \
	_dom_node_set_prefix, \
	_dom_node_get_local_name, \
	_dom_element_has_attributes, /*overload*/\
	_dom_node_get_base, \
	_dom_node_compare_document_position, \
	_dom_node_get_text_content, \
	_dom_node_set_text_content, \
	_dom_node_is_same, \
	_dom_element_lookup_prefix, /*overload*/\
	_dom_element_is_default_namespace, /*overload*/\
	_dom_element_lookup_namespace, /*overload*/\
	_dom_node_is_equal, \
	_dom_node_get_feature, \
	_dom_node_set_user_data, \
	_dom_node_get_user_data


/* The protected virtual function */
void __dom_element_destroy(dom_node_internal *node);
dom_exception _dom_element_alloc(struct dom_document *doc, 
		struct dom_node_internal *n, struct dom_node_internal **ret);
dom_exception _dom_element_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old);

#define DOM_ELEMENT_PROTECT_VTABLE \
	__dom_element_destroy, \
	_dom_element_alloc, \
	_dom_element_copy

/* Helper functions*/
dom_exception _dom_element_get_id(struct dom_element *ele, 
		struct lwc_string_s **id);

#endif
