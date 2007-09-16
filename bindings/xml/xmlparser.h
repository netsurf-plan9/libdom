/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef xml_xmlparser_h_
#define xml_xmlparser_h_

#include <stddef.h>
#include <inttypes.h>

#include "xmlerror.h"
#include "functypes.h"

struct dom_document;

typedef struct xml_parser xml_parser;

/* Create an XML parser instance */
xml_parser *xml_parser_create(const char *enc, const char *int_enc,
		xml_alloc alloc, void *pw, xml_msg msg, void *mctx);

/* Destroy an XML parser instance */
void xml_parser_destroy(xml_parser *parser);

/* Parse a chunk of data */
xml_error xml_parser_parse_chunk(xml_parser *parser,
		uint8_t *data, size_t len);

/* Notify parser that datastream is empty */
xml_error xml_parser_completed(xml_parser *parser);

/* Retrieve the created DOM Document */
struct dom_document *xml_parser_get_document(xml_parser *parser);

#endif
