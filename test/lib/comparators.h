/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 James Shaw <jshaw@netsurf-browser.org>
 */

#ifndef comparators_h_
#define comparators_h_

/**
 * A function pointer type for a comparator.
 */
typedef int (*comparator)(const void* a, const void* b);

int int_comparator(const int* a, const int* b);

#endif
