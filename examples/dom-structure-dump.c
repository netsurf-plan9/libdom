/*
 * This file is part of LibDOM.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 *
 * Copyright 2010 Michael Drake <tlsa@netsurf-browser.org>
 */

/*
 * Load an HTML file into LibDOM with Hubbub and print out the DOM structure.
 *
 * This example demonstrates the following:
 *
 * 1. Using LibDOM's Hubbub binding to read an HTML file into LibDOM.
 * 2. Walking around the DOM tree.
 * 3. Accessing DOM node attributes.
 * 4. Using LibWapcaplet to intern and compare strings with strings in the DOM.
 *
 * Example input:
 *      <html><body><h1 class="woo">NetSurf</h1>
 *      <p>NetSurf is <em>awesome</em>!</p>
 *      <div><h2>Hubbub</h2><p>Hubbub is too.</p>
 *      <p>Big time.</p></div></body></html>
 *
 * Example output:
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
	dom_exception exc; /* returned by libdom functions */
	lwc_error err; /* returned by libwapacplet functions */
	dom_document *doc = NULL; /* document, loaded into libdom */
	dom_element *root = NULL; /* root element of document */

	/* Initialise the DOM library */
	exc = dom_initialise(test_realloc, NULL);
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

	/* Get root element */
	exc = dom_document_get_document_element(doc, &root);
	if (exc != DOM_NO_ERR) {
		printf("Exception raised for get_document_element\n");
 		return EXIT_FAILURE;
	} else if (root == NULL) {
		printf("Broken: root == NULL\n");
 		return EXIT_FAILURE;
	}

	/* Dump DOM structure */
	dump_dom_structure(root, 0);

	/* Finalise the DOM library */
	exc = dom_finalise();
	if (exc != DOM_NO_ERR) {
		printf("Failed to finalise DOM library.\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


/**
 * Walk though a DOM (sub)tree, in depth first order, printing DOM structure.
 *
 * \param node   The root node to start from
 * \param depth  The depth of 'node' in the (sub)tree
 */
dump_dom_structure(dom_node_internal *node, int depth)
{
	dom_exception exc;
	dom_node_internal *child;

	/* Print this node's entry */
	dump_dom_element(node, depth);

	/* Get the node's first child */
	exc = dom_node_get_first_child(node, &child);
	if (exc != DOM_NO_ERR) {
		printf("Exception raised for node_get_first_child\n");
		return EXIT_FAILURE;
	} else if (child != NULL) {
		/* node has children;  decend to children's depth */
		depth++;

		/* Loop though all node's children */
		do {
			/* Visit node's descendents */
			dump_dom_structure(child, depth);

			/* Go to next sibling */
			exc = dom_node_get_next_sibling(child, &child);
			if (exc != DOM_NO_ERR) {
				printf("Exception raised for "
						"node_get_next_sibling\n");
				return EXIT_FAILURE;
			}
		} while (child != NULL); /* No more children */
	}
}


/**
 * Print a line in a DOM structure dump for an element
 *
 * \param node   The node to dump
 * \param depth  The node's depth
 */
dump_dom_element(dom_node_internal *node, int depth)
{
	dom_exception exc;
	lwc_error err;
	dom_string *node_name = NULL;
	lwc_string *lwcstr = NULL;
	const char *string;
	dom_node_type type;
	int i;
	size_t length;

	/* Only interested in element nodes */
	exc = dom_node_get_node_type(node, &type);
	if (exc != DOM_NO_ERR) {
		printf("Exception raised for node_get_node_type\n");
		return EXIT_FAILURE;
	} else if (type != DOM_ELEMENT_NODE) {
 		return;
	}

	/* Get root element name */
	exc = dom_node_get_node_name(node, &node_name);
	if (exc != DOM_NO_ERR) {
		printf("Exception raised for get_node_name\n");
		return EXIT_FAILURE;
	} else if (node_name == NULL) {
		printf("Broken: root_name == NULL\n");
 		return EXIT_FAILURE;
	}

	/* Get element name's lwc_string */
	exc = dom_string_get_intern(node_name, &lwcstr);
	if (exc != DOM_NO_ERR) {
		printf("Exception raised for string_get_intern\n");
		return EXIT_FAILURE;
	}

	/* Print ASCII tree structure for current node */
	if (depth > 0) {
		for (i = 0; i < depth; i++) {
			printf("| ");
		}
		printf("+-");
	}

	/* Get string data and print element name */
	string = lwc_string_data(lwcstr);
	length = lwc_string_length(lwcstr);
	printf("%*s\n", length, string);
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

