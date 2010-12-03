/*
 * This file is part of LibDOM.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 *
 * Copyright 2010 Michael Drake <tlsa@netsurf-browser.org>
 */

/*
 * Load an html file into libdom with hubbub and print out the dom structure
 *
 * e.g. <html><body><h1 class="woo">NetSurf</h1>
 *      <p>NetSurf is <em>awesome</em>!</p>
 *      <div><h2>Hubbub</h2><p>Hubbub is too.</p>
 *      <p>Big time.</p></div></body></html>
 *
 * gives:
 *
 * HTML
 * +-BODY
 * | +-H1 class="woo"
 * | | +-"NetSurf"
 * | +-P
 * | | +-"NetSurf is "
 * | | +-EM
 * | | | +-"awesome"
 * | | +-"!"
 * | +-DIV
 * | | +-H2
 * | | | +-"Hubbub"
 * | | +-P
 * | | | +-"Hubbub is too."
 * | | +-P
 * | | | +-"Big time."
 *
 *
 * or maybe just:
 *
 * HTML
 * +-BODY
 * | +-H1 class="woo"
 * | +-P
 * | | +-EM
 * | +-DIV
 * | | +-H2
 * | | +-P
 * | | +-P
 *
 */

#define _GNU_SOURCE /* for strndup */
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <dom/dom.h>
#include <dom/bindings/hubbub/parser.h>

#define UNUSED(x) ((x)=(x))

struct test_data {
	int wah;
};

dom_document *create_doc_dom_from_file(char *file);

void *test_realloc(void *ptr, size_t len, void *pw)
{
	UNUSED(pw);

	return realloc(ptr, len);
}

void test_msg(uint32_t severity, void *ctx, const char *msg, ...)
{
	va_list l;

	UNUSED(ctx);

	va_start(l, msg);

	fprintf(stderr, "%d: ", severity);
	vfprintf(stderr, msg, l);
	fprintf(stderr, "\n");
}

int main(int argc, char **argv)
{
	struct test_data test_data;
	lwc_error err;
	dom_exception exc;
	dom_document *doc = NULL;
	dom_element *root = NULL;
	dom_string *root_name = NULL;
	lwc_string *lwcstr = NULL;
	const char *string;

	/* Initialise the DOM library */
	exc = dom_initialise(test_realloc, &test_data);
	if (exc != DOM_NO_ERR) {
		printf("Failed to initialise DOM library.\n");
		return EXIT_FAILURE;
	}

	/* Initialise the string library */
	err = lwc_initialise (test_realloc, NULL, 0);
	if (err != lwc_error_ok) {
		printf("Failed to initialise string library.\n");
		return EXIT_FAILURE;
	}

	/* Load up the input HTML file */
	doc = create_doc_dom_from_file("files/test.html");
	if (doc == NULL) {
		printf("Failed to load document.\n");
		return EXIT_FAILURE;
	}

	/* Dump DOM structure */
	/* TODO: Actually just root element for now! */

	/* Get root element */
	exc = dom_document_get_document_element(doc, &root);
	if (exc != DOM_NO_ERR) {
		printf("Exception raised for get_document_element\n");
 		return EXIT_FAILURE;
	} else if (root == NULL) {
		printf("Broken: root == NULL\n");
 		return EXIT_FAILURE;
	}

	/* Get root element name */
	exc = dom_node_get_node_name(root, &root_name);
	if (exc != DOM_NO_ERR) {
		printf("Exception raised for get_node_name\n");
		return EXIT_FAILURE;
	} else if (root_name == NULL) {
		printf("Broken: root_name == NULL\n");
 		return EXIT_FAILURE;
	}

	/* Get root element name's lwc_string */
	exc = dom_string_get_intern(root_name, &lwcstr);
	if (exc != DOM_NO_ERR) {
		printf("Exception raised for string_get_intern\n");
		return EXIT_FAILURE;
	}

	/* Get string data and print */
	string = lwc_string_data(lwcstr);
	printf("%4s\n", string);

	/* Finalise the DOM library */
	exc = dom_finalise();
	if (exc != DOM_NO_ERR) {
		printf("Failed to finalise DOM library.\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}



/**
 * Generate a LibDOM document DOM from an HTML file
 *
 * \param file  The file path
 */
dom_document *create_doc_dom_from_file(char *file)
{
	const unsigned int buffer_size = 1024;
	dom_hubbub_parser *parser = NULL;
	int handle;
	int chunk_length;
	dom_hubbub_error error;
	dom_document *doc;
	char buffer[buffer_size];

	/* Create Hubbub parser */
	parser = dom_hubbub_parser_create("../test/data/Aliases", NULL, true,
			test_realloc, NULL, test_msg, NULL);
	if (parser == NULL) {
		printf("Can't create Hubbub Parser\n");
		return NULL;
	}

	/* Open input file */
	handle = open(file, O_RDONLY);
	if (handle == -1) {
		dom_hubbub_parser_destroy(parser);
		printf("Can't open test input file: %s\n", file);
		return NULL;
	}

	/* Parse input file in chunks */
	chunk_length = buffer_size;
	while(chunk_length == buffer_size) {
		chunk_length = read(handle, buffer, buffer_size);
		error = dom_hubbub_parser_parse_chunk(parser, buffer,
				chunk_length);
		if (error != DOM_HUBBUB_OK) {
			dom_hubbub_parser_destroy(parser);
			printf("Parsing errors occur\n");
			return NULL;
		}
	}

	/* Done parsing file */
	error = dom_hubbub_parser_completed(parser);
	if (error != DOM_HUBBUB_OK) {
		dom_hubbub_parser_destroy(parser);
		printf("Parsing error when construct DOM\n");
		return NULL;
	}

	/* Get the document */
	doc = dom_hubbub_parser_get_document(parser);

	/* Finished with parser */
	dom_hubbub_parser_destroy(parser);

	/* Close input file */
	if (close(handle) == -1) {
		printf("Can't close test input file: %s\n", file);
		return NULL;
	}

	return doc;
}

