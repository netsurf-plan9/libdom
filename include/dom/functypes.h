/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_functypes_h_
#define dom_functypes_h_

#include <stddef.h>

/**
 * Type of allocation function for DOM implementation
 */
typedef void *(*dom_alloc)(void *ptr, size_t size, void *pw);

#endif
