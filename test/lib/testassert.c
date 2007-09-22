/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <stdio.h>
#include <stdlib.h>

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
	assert_not_null(expected);
	assert_not_null(actual);
	assert_equals(expected->size, actual->size, (int (*)(const void* a, const void* b)) int_comparator);
	list_contains_all(actual, expected, comparator);
}

void assert_equals(int expected, int actual, comparator comparator)
{
	assert(comparator(&expected, &actual) == 0);
}

void assert_not_null(void* x)
{
	assert(x != NULL);
}

