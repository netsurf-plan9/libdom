/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_hubbub_parser_h_
#define dom_hubbub_parser_h_

#include <stddef.h>
#include <inttypes.h>

#include <dom/dom.h>

#include "errors.h"

struct dom_document;

typedef struct dom_hubbub_parser dom_hubbub_parser;

/* Create a Hubbub parser instance */
dom_hubbub_parser *dom_hubbub_parser_create(const char *enc, 
		const char *int_enc, dom_alloc alloc, void *pw, 
		dom_msg msg, void *mctx);

/* Destroy a Hubbub parser instance */
void dom_hubbub_parser_destroy(dom_hubbub_parser *parser);

/* Parse a chunk of data */
dom_hubbub_error dom_hubbub_parser_parse_chunk(dom_hubbub_parser *parser,
		uint8_t *data, size_t len);

/* Notify parser that datastream is empty */
dom_hubbub_error dom_hubbub_parser_completed(dom_hubbub_parser *parser);

/* Retrieve the created DOM Document */
struct dom_document *dom_hubbub_parser_get_document(dom_hubbub_parser *parser);

#endif
