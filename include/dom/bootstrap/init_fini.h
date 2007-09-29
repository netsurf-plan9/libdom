/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_bootstrap_init_fini_h_
#define dom_bootstrap_init_fini_h_

#include <dom/functypes.h>
#include <dom/core/exceptions.h>

/* Initialise the DOM library */
dom_exception dom_initialise(dom_alloc alloc, void *pw);

/* Finalise the DOM library */
dom_exception dom_finalise(void);

#endif

