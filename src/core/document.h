/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_document_h_
#define dom_internal_document_h_

#include <inttypes.h>
#include <stddef.h>

#include <dom/core/node.h>

struct dom_document;
struct dom_namednodemap;
struct dom_node;
struct dom_nodelist;
struct dom_string;

/* Get base of document buffer */
const uint8_t *dom_document_get_base(struct dom_document *doc);

/* (De)allocate memory */
void *dom_document_alloc(struct dom_document *doc, void *ptr, size_t size);

/* Get a nodelist, creating one if necessary */
dom_exception dom_document_get_nodelist(struct dom_document *doc,
		struct dom_node *root, struct dom_string *tagname,
		struct dom_string *namespace, struct dom_string *localname,
		struct dom_nodelist **list);
/* Remove a nodelist */
void dom_document_remove_nodelist(struct dom_document *doc,
		struct dom_nodelist *list);

/* Get a namednodemap, creating one if necessary */
dom_exception dom_document_get_namednodemap(struct dom_document *doc,
		struct dom_node *head, dom_node_type type,
		struct dom_namednodemap **map);
/* Remove a namednodemap */
void dom_document_remove_namednodemap(struct dom_document *doc,
		struct dom_namednodemap *map);

#endif
