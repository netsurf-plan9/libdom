/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2007 James Shaw <jshaw@netsurf-browser.org>
 */

#include <stdio.h>
#include <stdlib.h>

#include <dom/core/string.h>

#include "testassert.h"
#include "comparators.h"
#include "utils.h"

void __assert2(const char *expr, const char *function,
		const char *file, int line)
{
	UNUSED(function);
	UNUSED(file);

	printf("FAIL - %s at line %d\n", expr, line);

	exit(EXIT_FAILURE);
}

void assert_equals_collection(struct list* expected, struct list* actual,
		comparator comparator)
{
	assert(expected != NULL);
	assert(actual != NULL);
	assert_equals(&(expected->size), &(actual->size), (int (*)(const void* a, const void* b)) int_comparator);
	list_contains_all(actual, expected, comparator);
}

void assert_equals(void* expected, void* actual, comparator comparator)
{
	assert(comparator(expected, actual) == 0);
}

void assert_same(void* expected, void* actual, comparator comparator)
{
	if (!(expected == actual)) {
		assert_equals(expected, actual, comparator);
	}
}
void assert_uri_equals(char* scheme, char* path, char* host, char* file,
		     char* name, char* query, char* fragment, bool isAbsolute,
		     char* actual)
{
	UNUSED(scheme);
	UNUSED(path);
	UNUSED(host);
	UNUSED(file);
	UNUSED(name);
	UNUSED(query);
	UNUSED(fragment);
	UNUSED(isAbsolute);
	UNUSED(actual);
	/* TODO: implement me.  Look at netsurf/url.c */
}

