/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dom/bootstrap/init_fini.h>

#include "bindings/hubbub/parser.h"

#include "bindings/xml/xmlbinding.h"
#include "bindings/xml/xmlparser.h"

#include "testassert.h"
#include "testobject.h"
#include "utils.h"

static bool parser_initialised;

struct TestObject {
	enum { OBJECT_XML, OBJECT_HTML } type;
	union {
		dom_xml_parser *xml;
		dom_hubbub_parser *html;
	} parser;
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
	char *dot;
	size_t len;
	TestObject *ret;

	UNUSED(will_be_modified);

	if (argc != 2) {
		printf("Usage: %s <datapath>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (parser_initialised == false) {
		assert(dom_initialise(myrealloc, NULL) == DOM_NO_ERR);

		assert(dom_xml_binding_initialise(myrealloc, NULL) == 
				DOM_XML_OK);

//		assert(dom_hubbub_binding_initialise(myrealloc, NULL) ==
//				DOM_HUBBUB_OK);

		atexit(test_object_cleanup);

		parser_initialised = true;
	}

	snprintf(fnbuf, sizeof fnbuf, "%s/%s", argv[1], uri);

	ret = malloc(sizeof(TestObject));
	if (ret == NULL)
		return NULL;

	/* Detect the parser type (this is mildly hacky) */
	dot = strrchr(uri, '.');
	len = strlen(uri);

	if (dot == NULL) {
		printf("No file extension, assuming XML\n");

		ret->type = OBJECT_XML;
	} else if (len - ((dot + 1) - uri) == 3) {
		if (tolower(dot[1]) == 'x' && tolower(dot[2]) == 'm' 
				&& tolower(dot[3]) == 'l') {
			ret->type = OBJECT_XML;
		} else if (tolower(dot[1]) == 'h' && tolower(dot[2]) == 't' && 
				tolower(dot[3]) == 'm') {
			ret->type = OBJECT_HTML;
		}
	} else if (len - ((dot + 1) - uri) == 4) {
		if (tolower(dot[1]) == 'h' && tolower(dot[2]) == 't' &&
				tolower(dot[3]) == 'm' &&
				tolower(dot[4]) == 'l') {
			ret->type = OBJECT_HTML;
		}
	} else {
		/* Assume XML */
		ret->type = OBJECT_XML;
	}

	switch (ret->type) {
	case OBJECT_XML:
		ret->parser.xml = dom_xml_parser_create(NULL, "UTF-8", 
				myrealloc, NULL, mymsg, NULL);
		if (ret->parser.xml == NULL) {
			free(ret);
			return NULL;
		}
		break;
	case OBJECT_HTML:
		{
		char abuf[1024];
		snprintf(abuf, sizeof abuf, "%s/Aliases", argv[1]);

		ret->parser.html = dom_hubbub_parser_create(abuf, 
				NULL, true, myrealloc, NULL, mymsg, NULL);
		if (ret->parser.html == NULL) {
			free(ret);
			return NULL;
		}
		break;
		}
	}

	fp = fopen(fnbuf, "r");
	if (fp == NULL) {
		switch (ret->type) {
		case OBJECT_XML:
			dom_xml_parser_destroy(ret->parser.xml);
			break;
		case OBJECT_HTML:
			dom_hubbub_parser_destroy(ret->parser.html);
			break;
		}
		free(ret);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	while (len > CHUNK_SIZE) {
		fread(buf, 1, CHUNK_SIZE, fp);

		switch (ret->type) {
		case OBJECT_XML:
			assert(dom_xml_parser_parse_chunk(ret->parser.xml, 
					buf, CHUNK_SIZE) == DOM_XML_OK);
			break;
		case OBJECT_HTML:
			assert(dom_hubbub_parser_parse_chunk(ret->parser.html,
					buf, CHUNK_SIZE) == DOM_HUBBUB_OK);
			break;
		}

		len -= CHUNK_SIZE;
	}

	if (len > 0) {
		fread(buf, 1, len, fp);

		switch (ret->type) {
		case OBJECT_XML:
			assert(dom_xml_parser_parse_chunk(ret->parser.xml, 
					buf, len) == DOM_XML_OK);
			break;
		case OBJECT_HTML:
			assert(dom_hubbub_parser_parse_chunk(ret->parser.html,
					buf, len) == DOM_HUBBUB_OK);
			break;
		}

		len = 0;
	}

	switch (ret->type) {
	case OBJECT_XML:
		assert(dom_xml_parser_completed(ret->parser.xml) == DOM_XML_OK);
		break;
	case OBJECT_HTML:
		assert(dom_hubbub_parser_completed(ret->parser.html) ==
				DOM_HUBBUB_OK);
		break;
	}

	fclose(fp);

	switch (ret->type) {
	case OBJECT_XML:
		ret->doc = dom_xml_parser_get_document(ret->parser.xml);
		break;
	case OBJECT_HTML:
		ret->doc = dom_hubbub_parser_get_document(ret->parser.html);
		break;
	}

	switch (ret->type) {
	case OBJECT_XML:
		dom_xml_parser_destroy(ret->parser.xml);
		ret->parser.xml = NULL;
		break;
	case OBJECT_HTML:
		dom_hubbub_parser_destroy(ret->parser.html);
		ret->parser.html = NULL;
		break;
	}

	return ret;

#undef CHUNK_SIZE
}

struct dom_document *test_object_get_doc(TestObject *obj)
{
	return obj->doc;
}

const char *test_object_get_mimetype(TestObject *obj)
{
	return (obj->type == OBJECT_XML ? "text/xml" : "text/html");
}

void test_object_cleanup(void)
{
	if (parser_initialised) {
//		dom_hubbub_binding_finalise();
		dom_xml_binding_finalise();
		dom_finalise();
	}
}

