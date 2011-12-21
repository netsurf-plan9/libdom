/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <dom/dom.h>

#include "domts.h"

void __assert2(const char *expr, const char *function,
		const char *file, int line)
{
	UNUSED(function);
	UNUSED(file);

	printf("FAIL - %s at line %d\n", expr, line);

	exit(EXIT_FAILURE);
}

/**
 * Following are the test conditions which defined in the DOMTS, please refer
 * the DOM Test Suite for details
 */

bool is_true(bool arg)
{
	return arg == true;
}

bool is_null(void *arg)
{
	return arg == NULL;
}

bool is_same(void *expected, void *actual)
{
	return expected == actual;
}

bool is_same_int(int expected, int actual)
{
	return expected == actual;
}

bool is_same_unsigned_long(unsigned long expected, unsigned long actual)
{
	return expected == actual;
}

bool is_equals_int(int expected, int actual, bool dummy)
{
	UNUSED(dummy);
	
	return expected == actual;
}

bool is_equals_bool(bool expected, bool actual, bool dummy)
{
	UNUSED(dummy);

	return expected == actual;
}

bool is_equals_unsigned_long(unsigned long expected, unsigned long actual, bool dummy)
{
	UNUSED(dummy);

	return expected == actual;
}

/**
 * Test whether two string are equal
 * 
 * \param expected	The expected string
 * \param actual	The actual string
 * \param ignoreCase	Whether to ignore letter case
 */
bool is_equals_string(const char *expected, dom_string *actual, 
		bool ignoreCase)
{
	dom_string *exp;
	dom_exception err;
	bool ret;

	err = dom_string_create((const uint8_t *)expected, strlen(expected),
			&exp);
	if (err != DOM_NO_ERR)
		return false;

	if (ignoreCase == true)
		ret = dom_string_caseless_isequal(exp, actual);
	else
		ret = dom_string_isequal(exp, actual);
	
	dom_string_unref(exp);
	return ret;
}

/* Compare whether two dom_string are equal */
bool is_equals_domstring(dom_string *expected, dom_string *actual, 
		bool ignoreCase)
{
	if (ignoreCase == true)
		return dom_string_caseless_isequal(expected, actual);
	else
		return dom_string_isequal(expected, actual);
}

/* The param actual should always contain dom_sting and expectd should
 * contain char * */
bool is_equals_list(list *expected, list *actual, bool ignoreCase)
{
	assert((expected->type && 0xff00) == (actual->type && 0xff00));

	comparator cmp = NULL;
	comparator rcmp = NULL;

	if (expected->type == INT)
		cmp = int_comparator;
	if (expected->type == STRING) {
		if (actual->type == DOM_STRING) {
			cmp = ignoreCase? str_icmp : str_cmp;
			rcmp = ignoreCase? str_icmp_r : str_cmp_r;
		}
	}
	if (expected->type == DOM_STRING) {
		if (actual->type == STRING) {
			cmp = ignoreCase? str_icmp_r : str_cmp_r;
			rcmp = ignoreCase? str_icmp : str_cmp;
		}
	}

	assert(cmp != NULL);

	return list_contains_all(expected, actual, cmp) && list_contains_all(actual, expected, rcmp);
}



bool is_instanceof(const char *type, dom_node *node)
{
	assert("There is no instanceOf in the test-suite" == NULL);
        
        (void)type;
        (void)node;
        
	return false;
}


bool is_size_domnamednodemap(unsigned long size, dom_namednodemap *map)
{
	unsigned long len;
	dom_exception err;

	err = dom_namednodemap_get_length(map, &len);
	if (err != DOM_NO_ERR) {
		assert("Exception occured" == NULL);
		return false;
	}

	return size == len;
}

bool is_size_domnodelist(unsigned long size, dom_nodelist *list)
{
	unsigned long len;
	dom_exception err;

	err = dom_nodelist_get_length(list, &len);
	if (err != DOM_NO_ERR) {
		assert("Exception occured" == NULL);
		return false;
	}

	return size == len;
}

bool is_size_list(unsigned long size, list *list)
{
	return size == list->size;
}


bool is_uri_equals(char *scheme, char *path, char *host, 
		char *file, char *query, char *fragment, 
		bool isAbsolute, dom_string *actual)
{
	UNUSED(scheme);
	UNUSED(path);
	UNUSED(host);
	UNUSED(file);
	UNUSED(query);
	UNUSED(fragment);
	UNUSED(isAbsolute);
	UNUSED(actual);

	return false;
}


bool is_contenttype(const char *type)
{
	/* Now, we use the libxml2 parser for DOM parsing, so the content type
	 * is always "text/xml" */
	if (strcmp(type, "text/xml") == 0)
		return true;
	else
		return false;
}

bool has_feature(const char *feature, const char *version)
{
	dom_exception err;
	bool ret;

	if (feature == NULL)
		feature = "";

	if (version == NULL)
		version = "";

	err = dom_implementation_has_feature(feature, version, &ret);
	/* Here, when we come with exception, we should return false,
	 * TODO: this need to be improved, but I can't figure out how */
	if (err != DOM_NO_ERR) {
		return false;
	}

	return ret;
}

bool implementation_attribute(char *name, bool value)
{
	/* We didnot support DOMConfigure for implementation now */
	UNUSED(name);
	UNUSED(value);

	return true;
}
