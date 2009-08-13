/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_node_h_
#define dom_internal_core_node_h_

#include <stdbool.h>

#include <libwapcaplet/libwapcaplet.h>

#include <dom/core/node.h>
#include <dom/functypes.h>

#include "events/event_target.h"
#include "events/mutation_event.h"

#include "utils/list.h"

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
typedef struct dom_user_data dom_user_data;

/**
 * The internally used virtual function table.
 */
typedef struct dom_node_protect_vtable {

	void (*destroy)(dom_node_internal *n);
					/**< The destroy virtual function, it 
					 * should be private to client */
	dom_exception (*alloc)(struct dom_document *doc, 
			dom_node_internal *n, dom_node_internal **ret);
				/**< Allocate the memory of the new Node */
	dom_exception (*copy)(dom_node_internal *new, dom_node_internal *old);
				/**< Copy the old to new as well as 
				 * all its attributes, but not its children */
} dom_node_protect_vtable; 

/**
 * The real DOM node object
 *
 * DOM nodes are reference counted
 */
struct dom_node_internal {
	struct dom_node base;		/**< The vtable base */
	void *vtable;			/**< The protected vtable */

	struct lwc_string_s *name;	/**< Node name (this is the local part
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

	struct lwc_string_s *namespace;	/**< Namespace URI */
	struct lwc_string_s *prefix;	/**< Namespace prefix */

	struct dom_user_data *user_data;	/**< User data list */

	uint32_t refcnt;		/**< Reference count */

	struct list_entry pending_list; /**< The document delete pending list */

	dom_event_target_internal eti;	/**< The EventTarget interface */
};

dom_node_internal * _dom_node_create(struct dom_document *doc);

dom_exception _dom_node_initialise(struct dom_node_internal *node,
		struct dom_document *doc, dom_node_type type,
		struct lwc_string_s *name, struct dom_string *value,
		struct lwc_string_s *namespace, struct lwc_string_s *prefix);

dom_exception _dom_node_initialise_generic(
		struct dom_node_internal *node, struct dom_document *doc,
		dom_alloc alloc, void *pw, struct lwc_context_s *ctx,
	 	dom_node_type type, struct lwc_string_s *name, 
		struct dom_string *value, struct lwc_string_s *namespace, 
		struct lwc_string_s *prefix);

void _dom_node_finalise(struct dom_document *doc, dom_node_internal *node);
void _dom_node_finalise_generic(dom_node_internal *node, dom_alloc alloc, 
		void *pw, struct lwc_context_s *ctx);

bool _dom_node_readonly(const dom_node_internal *node);

/* The DOM Node's vtable methods */
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
		struct dom_string *feature, struct dom_string *version,
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
dom_exception _dom_node_is_same(dom_node_internal *node,
		dom_node_internal *other, bool *result);
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


/* Following comes the protected vtable */
void _dom_node_destroy(struct dom_node_internal *node);
dom_exception _dom_node_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret);
dom_exception _dom_node_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old);

#define DOM_NODE_PROTECT_VTABLE \
	_dom_node_destroy, \
	_dom_node_alloc, \
	_dom_node_copy


/* The destroy API should be used inside DOM module */
static inline void dom_node_destroy(struct dom_node_internal *node)
{
	((dom_node_protect_vtable *) node->vtable)->destroy(node);
}
#define dom_node_destroy(n) dom_node_destroy((dom_node_internal *) (n))

/* Allocate the Node */
static inline dom_exception dom_node_alloc(struct dom_document *doc, 
		struct dom_node_internal *n, struct dom_node_internal **ret)
{
	return ((dom_node_protect_vtable *) n->vtable)->alloc(doc, n, ret);
}
#define dom_node_alloc(d,n,r) dom_node_alloc((struct dom_document *) (d), \
		(dom_node_internal *) (n), (dom_node_internal **) (r))


/* Copy the Node old to new */
static inline dom_exception dom_node_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old)
{
	return ((dom_node_protect_vtable *) old->vtable)->copy(new, old);
}
#define dom_node_copy(n,o) dom_node_copy((dom_node_internal *) (n), \
		(dom_node_internal *) (o))

/* Following are some helper functions */
#define dom_node_get_owner(n) ((dom_node_internal *) (n))->owner

#define dom_node_set_owner(n, d) ((dom_node_internal *) (n))->owner = \
		(struct dom_document *) (d)

#define dom_node_get_parent(n) ((dom_node_internal *) (n))->parent

#define dom_node_set_parent(n, p) ((dom_node_internal *) (n))->parent = \
		(dom_node_internal *) (p)

#define dom_node_get_refcount(n) ((dom_node_internal *) (n))->refcnt

dom_exception _redocument_lwcstring(lwc_context *old, lwc_context *new, 
		lwc_string **string);
dom_exception _redocument_domstring(struct dom_document *old, 
		struct dom_document* new, struct dom_string **string);
dom_exception _dom_merge_adjacent_text(dom_node_internal *p,
		dom_node_internal *n);
/* Used to extract the lwc_string from dom_string.
 * If there is no lwc_string inside the param, create one use the node->owner
 * as document */
dom_exception _dom_node_get_intern_string(dom_node_internal *node, 
		struct dom_string *str, struct lwc_string_s **intern);
void _dom_node_unref_intern_string(dom_node_internal *node, 
		struct lwc_string_s *inter);

/* Try to destroy the node, if its refcnt is not zero, then append it to the
 * owner document's pending list */
void _dom_node_try_destroy(dom_node_internal *node);
#define dom_node_try_destroy(n) _dom_node_try_destroy((dom_node_internal *) (n))

/* To add some node to the pending list */
void _dom_node_mark_pending(dom_node_internal *node);
#define dom_node_mark_pending(n) _dom_node_mark_pending(\
		(dom_node_internal *) (n))
/* To remove the node from the pending list, this may happen when
 * a node is removed and then appended to another parent */
void _dom_node_remove_pending(dom_node_internal *node);
#define dom_node_remove_pending(n) _dom_node_remove_pending(\
		(dom_node_internal *) (n))

#endif
