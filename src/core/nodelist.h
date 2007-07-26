/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_nodelist_h_
#define dom_internal_core_nodelist_h_

#include <stdbool.h>

#include <dom/core/nodelist.h>

struct dom_document;
struct dom_node;
struct dom_nodelist;
struct dom_string;

/* Create a nodelist */
dom_exception dom_nodelist_create(struct dom_document *doc,
		struct dom_node *root, struct dom_string *tagname,
		struct dom_string *namespace, struct dom_string *localname,
		struct dom_nodelist **list);

/* Match a nodelist instance against a set of nodelist creation parameters */
bool dom_nodelist_match(struct dom_nodelist *list, struct dom_node *root,
		struct dom_string *tagname, struct dom_string *namespace,
		struct dom_string *localname);

#endif
