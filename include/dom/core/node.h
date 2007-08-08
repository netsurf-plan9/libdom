/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_node_h_
#define dom_core_node_h_

#include <inttypes.h>
#include <stdbool.h>

#include <dom/core/exceptions.h>

struct dom_document;
struct dom_node;
struct dom_nodelist;
struct dom_namednodemap;
struct dom_string;

/**
 * Bits defining position of a node in a document relative to some other node
 */
typedef enum {
	DOM_DOCUMENT_POSITION_DISCONNECTED		= 0x01,
	DOM_DOCUMENT_POSITION_PRECEDING			= 0x02,
	DOM_DOCUMENT_POSITION_FOLLOWING			= 0x04,
	DOM_DOCUMENT_POSITION_CONTAINS			= 0x08,
	DOM_DOCUMENT_POSITION_CONTAINED_BY		= 0x10,
	DOM_DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC	= 0x20
} dom_document_position;

/**
 * Type of node operation being notified to user_data_handler
 */
typedef enum {
	DOM_NODE_CLONED		= 1,
	DOM_NODE_IMPORTED	= 2,
	DOM_NODE_DELETED	= 3,
	DOM_NODE_RENAMED	= 4,
	DOM_NODE_ADOPTED	= 5
} dom_node_operation;

/**
 * Type of handler function for user data registered on a DOM node
 */
typedef void (*dom_user_data_handler)(dom_node_operation operation,
		struct dom_string *key, void *data, struct dom_node *src,
		struct dom_node *dst);

/**
 * Type of a DOM node
 */
typedef enum {
	DOM_ELEMENT_NODE		= 1,
	DOM_ATTRIBUTE_NODE		= 2,
	DOM_TEXT_NODE			= 3,
	DOM_CDATA_SECTION_NODE		= 4,
	DOM_ENTITY_REFERENCE_NODE	= 5,
	DOM_ENTITY_NODE			= 6,
	DOM_PROCESSING_INSTRUCTION_NODE	= 7,
	DOM_COMMENT_NODE		= 8,
	DOM_DOCUMENT_NODE		= 9,
	DOM_DOCUMENT_TYPE_NODE		= 10,
	DOM_DOCUMENT_FRAGMENT_NODE	= 11,
	DOM_NOTATION_NODE		= 12,

	/* And a count of the number of node types */
	DOM_NODE_TYPE_COUNT		= DOM_NOTATION_NODE
} dom_node_type;


void dom_node_ref(struct dom_node *node);
void dom_node_unref(struct dom_node *node);

dom_exception dom_node_get_node_name(struct dom_node *node,
		struct dom_string **result);
dom_exception dom_node_get_node_value(struct dom_node *node,
		struct dom_string **result);
dom_exception dom_node_set_node_value(struct dom_node *node,
		struct dom_string *value);
dom_exception dom_node_get_node_type(struct dom_node *node,
		dom_node_type *result);
dom_exception dom_node_get_parent_node(struct dom_node *node,
		struct dom_node **result);
dom_exception dom_node_get_child_nodes(struct dom_node *node,
		struct dom_nodelist **result);
dom_exception dom_node_get_first_child(struct dom_node *node,
		struct dom_node **result);
dom_exception dom_node_get_last_child(struct dom_node *node,
		struct dom_node **result);
dom_exception dom_node_get_previous_sibling(struct dom_node *node,
		struct dom_node **result);
dom_exception dom_node_get_next_sibling(struct dom_node *node,
		struct dom_node **result);
dom_exception dom_node_get_attributes(struct dom_node *node,
		struct dom_namednodemap **result);
dom_exception dom_node_get_owner_document(struct dom_node *node,
		struct dom_document **result);
dom_exception dom_node_insert_before(struct dom_node *node,
		struct dom_node *new_child, struct dom_node *ref_child,
		struct dom_node **result);
dom_exception dom_node_replace_child(struct dom_node *node,
		struct dom_node *new_child, struct dom_node *old_child,
		struct dom_node **result);
dom_exception dom_node_remove_child(struct dom_node *node,
		struct dom_node *old_child,
		struct dom_node **result);
dom_exception dom_node_append_child(struct dom_node *node,
		struct dom_node *new_child,
		struct dom_node **result);
dom_exception dom_node_has_child_nodes(struct dom_node *node, bool *result);
dom_exception dom_node_clone_node(struct dom_node *node, bool deep,
		struct dom_node **result);
dom_exception dom_node_normalize(struct dom_node *node);
dom_exception dom_node_is_supported(struct dom_node *node,
		struct dom_string *feature, struct dom_node *version,
		bool *result);
dom_exception dom_node_get_namespace(struct dom_node *node,
		struct dom_string **result);
dom_exception dom_node_get_prefix(struct dom_node *node,
		struct dom_string **result);
dom_exception dom_node_set_prefix(struct dom_node *node,
		struct dom_string *prefix);
dom_exception dom_node_get_local_name(struct dom_node *node,
		struct dom_string **result);
dom_exception dom_node_has_attributes(struct dom_node *node, bool *result);
dom_exception dom_node_get_base(struct dom_node *node,
		struct dom_string **result);
dom_exception dom_node_compare_document_position(struct dom_node *node,
		struct dom_node *other, uint16_t *result);
dom_exception dom_node_get_text_content(struct dom_node *node,
		struct dom_string **result);
dom_exception dom_node_set_text_content(struct dom_node *node,
		struct dom_string *content);
dom_exception dom_node_is_same(struct dom_node *node, struct dom_node *other,
		bool *result);
dom_exception dom_node_lookup_prefix(struct dom_node *node,
		struct dom_string *namespace, struct dom_string **result);
dom_exception dom_node_is_default_namespace(struct dom_node *node,
		struct dom_string *namespace, bool *result);
dom_exception dom_node_lookup_namespace(struct dom_node *node,
		struct dom_string *prefix, struct dom_string **result);
dom_exception dom_node_is_equal(struct dom_node *node,
		struct dom_node *other, bool *result);
dom_exception dom_node_get_feature(struct dom_node *node,
		struct dom_string *feature, struct dom_string *version,
		void **result);
dom_exception dom_node_set_user_data(struct dom_node *node,
		struct dom_string *key, void *data,
		dom_user_data_handler handler, void **result);
dom_exception dom_node_get_user_data(struct dom_node *node,
		struct dom_string *key, void **result);

#endif
