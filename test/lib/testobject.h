/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef testobject_h_
#define testobject_h_

#include <stdbool.h>

struct TestObject;
typedef struct TestObject TestObject;

TestObject *test_object_create(int argc, char **argv,
		const char *uri, bool will_be_modified);
struct dom_document *test_object_get_doc(TestObject *obj);
const char *test_object_get_mimetype(TestObject *obj);

#endif

