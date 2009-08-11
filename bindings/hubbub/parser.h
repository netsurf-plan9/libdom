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
struct lwc_context_s;

typedef struct dom_hubbub_parser dom_hubbub_parser;

/* The encoding source of the document */
typedef enum dom_hubub_encoding_source { 
	ENCODING_SOURCE_HEADER, 
	ENCODING_SOURCE_DETECTED,
	ENCODING_SOURCE_META 
} dom_hubbub_encoding_source;

/* The recommended way to use the parser is:
 * 
 * dom_hubbub_parser_create(...);
 * dom_hubbub_parser_parse_chunk(...);
 * call _parse_chunk for all chunks of data
 *
 * After you have parsed the data,
 *
 * dom_hubbub_parser_completed(...);
 * dom_bubbub_parser_get_document(...);
 * dom_hubbub_parser_destroy(...);
 *
 * Clients must ensure that the last 3 function calls above are called in
 * the order shown. dom_hubbub_parser_get_document() will pass the ownership 
 * of the document to the client. After that, the parser should be destroyed.
 * The client must not call any method of this parser after destruction.
 *
 * The client must call dom_hubbub_parser_completed() before calling 
 * dom_hubbub_parser_get_document().
 */

/* Create a Hubbub parser instance */
dom_hubbub_parser *dom_hubbub_parser_create(const char *aliases,
		const char *enc, bool fix_enc, 
		dom_alloc alloc, void *pw, dom_msg msg, void *mctx, 
        struct lwc_context_s *ctx);

/* Destroy a Hubbub parser instance */
void dom_hubbub_parser_destroy(dom_hubbub_parser *parser);

/* Parse a chunk of data */
dom_hubbub_error dom_hubbub_parser_parse_chunk(dom_hubbub_parser *parser,
		uint8_t *data, size_t len);

/* Notify parser that datastream is empty */
dom_hubbub_error dom_hubbub_parser_completed(dom_hubbub_parser *parser);

/* Retrieve the created DOM Document */
struct dom_document *dom_hubbub_parser_get_document(dom_hubbub_parser *parser);

/* Retrieve the document's encoding */
const char *dom_hubbub_parser_get_encoding(dom_hubbub_parser *parser, 
		dom_hubbub_encoding_source *source);

#endif
