/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_node_h_
#define dom_internal_core_node_h_

#include <dom/core/node.h>
#include <dom/core/string.h>

struct dom_attr;

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
 * DOM node object
 *
 * DOM nodes are reference counted
 */
struct dom_node {
	struct dom_string *name;	/**< Node name */
	struct dom_string *value;	/**< Node value */
	dom_node_type type;		/**< Node type */
	struct dom_node *parent;	/**< Parent node */
	struct dom_node *first_child;	/**< First child node */
	struct dom_node *last_child;	/**< Last child node */
	struct dom_node *previous;	/**< Previous sibling */
	struct dom_node *next;		/**< Next sibling */
	struct dom_attr *attributes;	/**< Node attributes */

	struct dom_document *owner;	/**< Owning document */

	struct dom_string *namespace;	/**< Namespace URI */
	struct dom_string *prefix;	/**< Namespace prefix */
	struct dom_string *localname;	/**< Local part of qualified name */

	struct dom_user_data *user_data;	/**< User data list */

	uint32_t refcnt;		/**< Reference count */
};

dom_exception dom_node_create(struct dom_document *doc, dom_node_type type,
		struct dom_string *name, struct dom_string *value,
		struct dom_node **node);

#endif
