/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <stdio.h>
#include <stdlib.h>

#include <dom/bootstrap/init_fini.h>

#include "bindings/xml/xmlbinding.h"
#include "bindings/xml/xmlparser.h"

#include "testassert.h"
#include "testobject.h"
#include "utils.h"

static bool xml_parser_initialised;

struct TestObject {
	dom_xml_parser *parser;
	struct dom_document *doc;
};

static void test_object_cleanup(void);

TestObject *test_object_create(int argc, char **argv,
		const char *uri, bool will_be_modified)
{
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
		assert(dom_initialise(myrealloc, NULL) == DOM_NO_ERR);

		assert(dom_xml_binding_initialise(myrealloc, NULL) == 
				DOM_XML_OK);

		atexit(test_object_cleanup);

		xml_parser_initialised = true;
	}

	snprintf(fnbuf, sizeof fnbuf, "%s/%s", argv[1], uri);

	ret = malloc(sizeof(TestObject));
	if (ret == NULL)
		return NULL;

	ret->parser = dom_xml_parser_create(NULL, "UTF-8", myrealloc, NULL,
			mymsg, NULL);
	if (ret->parser == NULL) {
		free(ret);
		return NULL;
	}

	fp = fopen(fnbuf, "r");
	if (fp == NULL) {
		dom_xml_parser_destroy(ret->parser);
		free(ret);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	while (len > CHUNK_SIZE) {
		fread(buf, 1, CHUNK_SIZE, fp);

		assert(dom_xml_parser_parse_chunk(ret->parser, buf,
				CHUNK_SIZE) == DOM_XML_OK);

		len -= CHUNK_SIZE;
	}

	if (len > 0) {
		fread(buf, 1, len, fp);

		assert(dom_xml_parser_parse_chunk(ret->parser, buf,
				len) == DOM_XML_OK);

		len = 0;
	}

	assert(dom_xml_parser_completed(ret->parser) == DOM_XML_OK);

	fclose(fp);

	ret->doc = dom_xml_parser_get_document(ret->parser);

	dom_xml_parser_destroy(ret->parser);
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

void test_object_cleanup(void)
{
	if (xml_parser_initialised) {
		dom_xml_binding_finalise();
		dom_finalise();
	}
}

