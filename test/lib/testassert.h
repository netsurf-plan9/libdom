/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef testassert_h_
#define testassert_h_

#include "comparators.h"
#include "list.h"

/* Redefine assert, so we can simply use the standard assert mechanism
 * within testcases and exit with the right output for the testrunner
 * to do the right thing. */
void __assert2(const char *expr, const char *function,
		const char *file, int line);

#define assert(expr) \
  ((void) ((expr) || (__assert2 (#expr, __func__, __FILE__, __LINE__), 0)))

void assert_equals_collection(struct list* expected, struct list* actual,
		comparator comparator);

void assert_equals(void* expected, void* actual, comparator comparator);

void assert_same(void* expected, void* actual, comparator comparator);

void assert_uri_equals(char* scheme, char* path, char* host, char* file,
		     char* name, char* query, char* fragment, bool isAbsolute,
		     char* actual);

#endif
