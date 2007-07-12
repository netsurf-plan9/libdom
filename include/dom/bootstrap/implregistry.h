/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_bootstrap_implregistry_h_
#define dom_bootstrap_implregistry_h_

#include <dom/core/exceptions.h>
#include <dom/functypes.h>

struct dom_implementation;
struct dom_implementation_list;
struct dom_string;

/* Retrieve a DOM implementation from the registry */
dom_exception dom_implregistry_get_dom_implementation(
		struct dom_string *features,
		struct dom_implementation **impl,
		dom_alloc alloc, void *pw);

/* Get a list of DOM implementations that support the requested features */
dom_exception dom_implregistry_get_dom_implementation_list(
		struct dom_string *features,
		struct dom_implementation_list **list,
		dom_alloc alloc, void *pw);

#endif
