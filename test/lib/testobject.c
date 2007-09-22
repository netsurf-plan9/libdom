/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <stdio.h>
#include <stdlib.h>

#include "bindings/xml/xmlbinding.h"
#include "bindings/xml/xmlparser.h"

#include "testassert.h"
#include "testobject.h"
#include "utils.h"

struct TestObject {
	xml_parser *parser;
	struct dom_document *doc;
};

TestObject *test_object_create(int argc, char **argv,
		const char *uri, bool will_be_modified)
{
	static bool xml_parser_initialised;

	char fnbuf[1024];
#define CHUNK_SIZE 4096
	uint8_t buf[CHUNK_SIZE];
	FILE *fp;
	size_t len;
	TestObject *ret;

	UNUSED(will_be_modified);

	if (argc != 2) {
		printf("Usage: %s <datapath>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (xml_parser_initialised == false) {
		assert(xml_dom_binding_initialise(myrealloc, NULL) == XML_OK);

		xml_parser_initialised = true;
	}

	snprintf(fnbuf, sizeof fnbuf, "%s/%s", argv[1], uri);

	ret = malloc(sizeof(TestObject));
	if (ret == NULL)
		return NULL;

	ret->parser = xml_parser_create(NULL, "UTF-8", myrealloc, NULL,
			mymsg, NULL);
	if (ret->parser == NULL) {
		free(ret);
		return NULL;
	}

	fp = fopen(fnbuf, "r");
	if (fp == NULL) {
		xml_parser_destroy(ret->parser);
		free(ret);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	while (len > CHUNK_SIZE) {
		fread(buf, 1, CHUNK_SIZE, fp);

		assert(xml_parser_parse_chunk(ret->parser, buf,
				CHUNK_SIZE) == XML_OK);

		len -= CHUNK_SIZE;
	}

	if (len > 0) {
		fread(buf, 1, len, fp);

		assert(xml_parser_parse_chunk(ret->parser, buf,
				len) == XML_OK);

		len = 0;
	}

	assert(xml_parser_completed(ret->parser) == XML_OK);

	fclose(fp);

	ret->doc = xml_parser_get_document(ret->parser);

	xml_parser_destroy(ret->parser);
	ret->parser = NULL;

	return ret;

#undef CHUNK_SIZE
}

struct dom_document *test_object_get_doc(TestObject *obj)
{
	return obj->doc;
}

const char *test_object_get_mimetype(TestObject *obj)
{
	UNUSED(obj);

	return "text/xml";
}


