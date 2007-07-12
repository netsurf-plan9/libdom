/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_impllist_h_
#define dom_core_impllist_h_

#include <dom/core/exceptions.h>

struct dom_implementation;
struct dom_implementation_list;

void dom_implementation_list_ref(struct dom_implementation_list *list);
void dom_implementation_list_unref(struct dom_implementation_list *list);

dom_exception dom_implementation_list_get_length(
		struct dom_implementation_list *list, unsigned long *length);

dom_exception dom_implementation_list_item(
		struct dom_implementation_list *list, unsigned long index,
		struct dom_implementation **impl);

#endif
