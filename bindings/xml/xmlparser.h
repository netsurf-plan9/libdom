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

#include <dom/dom.h>

#include "xmlerror.h"

struct dom_document;
struct lwc_context_s;

typedef struct dom_xml_parser dom_xml_parser;

/* Create an XML parser instance */
dom_xml_parser *dom_xml_parser_create(const char *enc, const char *int_enc,
		dom_alloc alloc, void *pw, dom_msg msg, void *mctx, 
		struct lwc_context_s *ctx);

/* Destroy an XML parser instance */
void dom_xml_parser_destroy(dom_xml_parser *parser);

/* Parse a chunk of data */
dom_xml_error dom_xml_parser_parse_chunk(dom_xml_parser *parser,
		uint8_t *data, size_t len);

/* Notify parser that datastream is empty */
dom_xml_error dom_xml_parser_completed(dom_xml_parser *parser);

/* Retrieve the created DOM Document */
struct dom_document *dom_xml_parser_get_document(dom_xml_parser *parser);

#endif
