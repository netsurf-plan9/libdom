/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

#include <libwapcaplet/libwapcaplet.h>

// For parsers
#include <dom/dom.h>
#include <xmlparser.h>
#include <parser.h>
#include <errors.h>

#include "utils.h"

/**
 * Load the file as it is a XML file
 *
 * \param file		The file path 
 * \param willBeModified	Whether this file will be modified, not used
 */
dom_document *load_xml(char *file, bool willBeModified)
{
	dom_xml_parser *parser = NULL;
	int handle;
	int readed;
	dom_xml_error error;
	dom_document *ret;
	char buffer[1024];

	UNUSED(willBeModified);

	parser = dom_xml_parser_create(NULL, NULL, 
			myrealloc, NULL, mymsg, NULL);
	if (parser == NULL) {
		fprintf(stderr, "Can't create XMLParser\n");
		return NULL;
	}

	handle = open(file, O_RDONLY);
	if (handle == -1) {
		dom_xml_parser_destroy(parser);
		fprintf(stderr, "Can't open test input file: %s\n", file);
		return NULL;
	}

	readed = read(handle, buffer, 1024);
	error = dom_xml_parser_parse_chunk(parser, buffer, readed);
	if (error != DOM_XML_OK) {
		dom_xml_parser_destroy(parser);
		fprintf(stderr, "Parsing errors occur\n");
		return NULL;
	}

	while(readed == 1024) {
		readed = read(handle, buffer, 1024);
		error = dom_xml_parser_parse_chunk(parser, buffer, readed);
		if (error != DOM_XML_OK) {
			dom_xml_parser_destroy(parser);
			fprintf(stderr, "Parsing errors occur\n");
			return NULL;
		}
	}

	error = dom_xml_parser_completed(parser);
	if (error != DOM_XML_OK) {
		dom_xml_parser_destroy(parser);
		fprintf(stderr, "Parsing error when construct DOM\n");
		return NULL;
	}

	ret = dom_xml_parser_get_document(parser);
	dom_xml_parser_destroy(parser);

	return ret;
}

/**
 * Load the file as it is a HTML file
 *
 * \param file		The file path 
 * \param willBeModified	Whether this file will be modified, not used
 */
dom_document *load_html(char *file, bool willBeModified)
{
	dom_hubbub_parser *parser = NULL;
	int handle;
	int readed;
	dom_hubbub_error error;
	dom_document *ret;
	char buffer[1024];

	UNUSED(willBeModified);

	parser = dom_hubbub_parser_create("../data/Aliases", NULL, true,
			myrealloc, NULL, mymsg, NULL);
	if (parser == NULL) {
		fprintf(stderr, "Can't create Hubbub Parser\n");
		return NULL;
	}

	handle = open(file, O_RDONLY);
	if (handle == -1) {
		dom_hubbub_parser_destroy(parser);
		/* fprintf(stderr, "Can't open test input file: %s\n", file); */
		return NULL;
	}

	readed = read(handle, buffer, 1024);
	error = dom_hubbub_parser_parse_chunk(parser, buffer, readed);
	if (error != DOM_HUBBUB_OK) {
		dom_hubbub_parser_destroy(parser);
		fprintf(stderr, "Parsing errors occur\n");
		return NULL;
	}

	while(readed == 1024) {
		readed = read(handle, buffer, 1024);
		error = dom_hubbub_parser_parse_chunk(parser, buffer, readed);
		if (error != DOM_HUBBUB_OK) {
			dom_hubbub_parser_destroy(parser);
			fprintf(stderr, "Parsing errors occur\n");
			return NULL;
		}
	}

	error = dom_hubbub_parser_completed(parser);
	if (error != DOM_HUBBUB_OK) {
		dom_hubbub_parser_destroy(parser);
		fprintf(stderr, "Parsing error when construct DOM\n");
		return NULL;
	}

	ret = dom_hubbub_parser_get_document(parser);
	dom_hubbub_parser_destroy(parser);

	return ret;
}
