/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_node_h_
#define dom_internal_core_node_h_

#include <stdbool.h>

#include <dom/core/node.h>

/**
 * User data context attached to a DOM node
 */
struct dom_user_data {
	struct dom_string *key;		/**< Key for data */
	void *data;			/**< Client-specific data */
	dom_user_data_handler handler;	/**< Callback function */

	struct dom_user_data *next;	/**< Next in list */
	struct dom_user_data *prev;	/**< Previous in list */
};

/**
 * The real DOM node object
 *
 * DOM nodes are reference counted
 */
struct dom_node_internal {
	struct dom_node base;		/**< The vtable base */
	void (*destroy)(dom_node_internal *n);
					/**< The destroy vitual function, it 
					 * should be privated to client */

	struct dom_string *name;	/**< Node name (this is the local part 
					 * of a QName in the cases where a 
					 * namespace exists) */
	struct dom_string *value;	/**< Node value */
	dom_node_type type;		/**< Node type */
	dom_node_internal *parent;	/**< Parent node */
	dom_node_internal *first_child;	/**< First child node */
	dom_node_internal *last_child;	/**< Last child node */
	dom_node_internal *previous;	/**< Previous sibling */
	dom_node_internal *next;		/**< Next sibling */

	struct dom_document *owner;	/**< Owning document */

	struct dom_string *namespace;	/**< Namespace URI */
	struct dom_string *prefix;	/**< Namespace prefix */

	struct dom_user_data *user_data;	/**< User data list */

	uint32_t refcnt;		/**< Reference count */
};

dom_node_internal * dom_node_create(struct dom_document *doc);

dom_exception dom_node_initialise(struct dom_node_internal *node,
		struct dom_document *doc, dom_node_type type,
		struct dom_string *name, struct dom_string *value,
		struct dom_string *namespace, struct dom_string *prefix);

void dom_node_finalise(struct dom_document *doc, dom_node_internal *node);

bool _dom_node_readonly(const dom_node_internal *node);

/* The DOM Node's vtable methods */
void _dom_node_destroy(struct dom_node_internal *node);
dom_exception _dom_node_get_node_name(dom_node_internal *node,
		struct dom_string **result);
dom_exception _dom_node_get_node_value(dom_node_internal *node,
		struct dom_string **result);
dom_exception _dom_node_set_node_value(dom_node_internal *node,
		struct dom_string *value);
dom_exception _dom_node_get_node_type(dom_node_internal *node,
		dom_node_type *result);
dom_exception _dom_node_get_parent_node(dom_node_internal *node,
		dom_node_internal **result);
dom_exception _dom_node_get_child_nodes(dom_node_internal *node,
		struct dom_nodelist **result);
dom_exception _dom_node_get_first_child(dom_node_internal *node,
		dom_node_internal **result);
dom_exception _dom_node_get_last_child(dom_node_internal *node,
		dom_node_internal **result);
dom_exception _dom_node_get_previous_sibling(dom_node_internal *node,
		dom_node_internal **result);
dom_exception _dom_node_get_next_sibling(dom_node_internal *node,
		dom_node_internal **result);
dom_exception _dom_node_get_attributes(dom_node_internal *node,
		struct dom_namednodemap **result);
dom_exception _dom_node_get_owner_document(dom_node_internal *node,
		struct dom_document **result);
dom_exception _dom_node_insert_before(dom_node_internal *node,
		dom_node_internal *new_child, dom_node_internal *ref_child,
		dom_node_internal **result);
dom_exception _dom_node_replace_child(dom_node_internal *node,
		dom_node_internal *new_child, dom_node_internal *old_child,
		dom_node_internal **result);
dom_exception _dom_node_remove_child(dom_node_internal *node,
		dom_node_internal *old_child,
		dom_node_internal **result);
dom_exception _dom_node_append_child(dom_node_internal *node,
		dom_node_internal *new_child,
		dom_node_internal **result);
dom_exception _dom_node_has_child_nodes(dom_node_internal *node, bool *result);
dom_exception _dom_node_clone_node(dom_node_internal *node, bool deep,
		dom_node_internal **result);
dom_exception _dom_node_normalize(dom_node_internal *node);
dom_exception _dom_node_is_supported(dom_node_internal *node,
		struct dom_string *feature, dom_node_internal *version,
		bool *result);
dom_exception _dom_node_get_namespace(dom_node_internal *node,
		struct dom_string **result);
dom_exception _dom_node_get_prefix(dom_node_internal *node,
		struct dom_string **result);
dom_exception _dom_node_set_prefix(dom_node_internal *node,
		struct dom_string *prefix);
dom_exception _dom_node_get_local_name(dom_node_internal *node,
		struct dom_string **result);
dom_exception _dom_node_has_attributes(dom_node_internal *node, bool *result);
dom_exception _dom_node_get_base(dom_node_internal *node,
		struct dom_string **result);
dom_exception _dom_node_compare_document_position(dom_node_internal *node,
		dom_node_internal *other, uint16_t *result);
dom_exception _dom_node_get_text_content(dom_node_internal *node,
		struct dom_string **result);
dom_exception _dom_node_set_text_content(dom_node_internal *node,
		struct dom_string *content);
dom_exception _dom_node_is_same(dom_node_internal *node, dom_node_internal *other,
		bool *result);
dom_exception _dom_node_lookup_prefix(dom_node_internal *node,
		struct dom_string *namespace, struct dom_string **result);
dom_exception _dom_node_is_default_namespace(dom_node_internal *node,
		struct dom_string *namespace, bool *result);
dom_exception _dom_node_lookup_namespace(dom_node_internal *node,
		struct dom_string *prefix, struct dom_string **result);
dom_exception _dom_node_is_equal(dom_node_internal *node,
		dom_node_internal *other, bool *result);
dom_exception _dom_node_get_feature(dom_node_internal *node,
		struct dom_string *feature, struct dom_string *version,
		void **result);
dom_exception _dom_node_set_user_data(dom_node_internal *node,
		struct dom_string *key, void *data,
		dom_user_data_handler handler, void **result);
dom_exception _dom_node_get_user_data(dom_node_internal *node,
		struct dom_string *key, void **result);

#define DOM_NODE_VTABLE \
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
	_dom_node_get_attributes, \
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
	_dom_node_has_attributes, \
	_dom_node_get_base, \
	_dom_node_compare_document_position, \
	_dom_node_get_text_content, \
	_dom_node_set_text_content, \
	_dom_node_is_same, \
	_dom_node_lookup_prefix, \
	_dom_node_is_default_namespace, \
	_dom_node_lookup_namespace, \
	_dom_node_is_equal, \
	_dom_node_get_feature, \
	_dom_node_set_user_data, \
	_dom_node_get_user_data


/* The destroy API should be used inside DOM module */
static inline void dom_node_destroy(struct dom_node *node)
{
	((dom_node_internal *) node)->destroy((dom_node_internal *) node);
}
#define dom_node_destroy(n) dom_node_destroy((dom_node *) (n))

#endif
