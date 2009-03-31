/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_nodelist_h_
#define dom_core_nodelist_h_

#include <dom/core/exceptions.h>

struct dom_node;

typedef struct dom_nodelist dom_nodelist;

void dom_nodelist_ref(struct dom_nodelist *list);
void dom_nodelist_unref(struct dom_nodelist *list);

dom_exception dom_nodelist_get_length(struct dom_nodelist *list,
		unsigned long *length);
dom_exception dom_nodelist_item(struct dom_nodelist *list,
		unsigned long index, struct dom_node **node);

#endif
