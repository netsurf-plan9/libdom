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

dom_exception dom_element_create(struct dom_document *doc,
		struct dom_string *name, struct dom_element **result);

void dom_element_destroy(struct dom_document *doc,
		struct dom_element *element);

dom_exception dom_element_get_attributes(struct dom_element *element,
		struct dom_namednodemap **result);

dom_exception dom_element_has_attributes(struct dom_element *element,
		bool *result);

struct dom_node *dom_element_get_first_attribute(struct dom_element *element);

#endif
