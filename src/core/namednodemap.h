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

struct dom_document;
struct dom_node;
struct dom_namednodemap;
struct dom_string;

/**
 * Type of a named node map
 */
typedef enum {
	DOM_NAMEDNODEMAP_ATTRIBUTES,
	DOM_NAMEDNODEMAP_ENTITIES,
	DOM_NAMEDNODEMAP_NOTATIONS
} dom_namednodemap_type;

/* Create a namednodemap */
dom_exception dom_namednodemap_create(struct dom_document *doc,
		struct dom_node *root, dom_namednodemap_type type,
		struct dom_namednodemap **map);


/* Match a namednodemap instance against a set of creation parameters */
bool dom_namednodemap_match(struct dom_namednodemap *map,
		struct dom_node *root, dom_namednodemap_type type);

#endif
