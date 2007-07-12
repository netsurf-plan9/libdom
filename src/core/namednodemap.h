/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_namednodemap_h_
#define dom_internal_namednodemap_h_

#include <stdbool.h>

#include <dom/core/namednodemap.h>
#include <dom/core/node.h>

struct dom_document;
struct dom_node;
struct dom_namednodemap;
struct dom_string;

/* Create a namednodemap */
dom_exception dom_namednodemap_create(struct dom_document *doc,
		struct dom_node *head, dom_node_type type,
		struct dom_namednodemap **map);


/* Match a namednodemap instance against a set of creation parameters */
bool dom_namednodemap_match(struct dom_namednodemap *map,
		struct dom_node *head, dom_node_type type);

#endif
