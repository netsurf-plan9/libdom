/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2012 Daniel Silverstone <dsilvers@netsurf-browser.org>
 */

#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <stdlib.h>
#include <stdio.h>

#include <dom/dom.h>

#include "xmlparser.h"
#include "utils.h"

#include <expat.h>

/**
 * expat XML parser object
 */
struct dom_xml_parser {
	dom_msg msg;			/**< Informational message function */
	void *mctx;			/**< Pointer to client data */
	XML_Parser parser;		/**< expat parser context */
	bool complete;			/**< Indicate stream completion */
	struct dom_document *doc;	/**< DOM Document we're building */
	struct dom_node *current;	/**< DOM node we're currently building */
};

/* Binding functions */

static void
expat_xmlparser_start_element_handler(void *_parser,
				      const XML_Char *name,
				      const XML_Char **atts)
{
	dom_xml_parser *parser = _parser;
	dom_exception err;
	dom_element *elem, *ins_elem;
	dom_string *tag_name;
	
	fprintf(stderr, "<%s>\n", name);
	
	err = dom_string_create((const uint8_t *)name,
				strlen(name),
				&tag_name);
	if (err != DOM_NO_ERR) {
		parser->msg(DOM_MSG_CRITICAL, parser->mctx,
			    "No memory for tag name");
		return;
	}
	
	err = dom_document_create_element(parser->doc, tag_name, &elem);
	if (err != DOM_NO_ERR) {
		dom_string_unref(tag_name);
		parser->msg(DOM_MSG_CRITICAL, parser->mctx,
			    "Failed to create element '%s'", name);
		return;
	}
	
	dom_string_unref(tag_name);
	
	/* Add attributes to the element */
	
	while (*atts) {
		dom_string *key, *value;

		err = dom_string_create((const uint8_t *)(*atts), 
					strlen(*atts), &key);
		if (err != DOM_NO_ERR) {
			parser->msg(DOM_MSG_CRITICAL, parser->mctx,
				    "No memory for attribute name");
			dom_node_unref(elem);
			return;
		}
		atts++;
		err = dom_string_create((const uint8_t *)(*atts),
					strlen(*atts), &value);
		if (err != DOM_NO_ERR) {
			dom_node_unref(elem);
			dom_string_unref(key);
			parser->msg(DOM_MSG_CRITICAL, parser->mctx,
				    "No memory for attribute value");
			return;
		}
		atts++;
		
		err = dom_element_set_attribute(elem, key, value);
		dom_string_unref(key);
		dom_string_unref(value);
		if (err != DOM_NO_ERR) {
			dom_node_unref(elem);
			parser->msg(DOM_MSG_CRITICAL, parser->mctx,
				    "No memory for setting attribute");
			return;
		}
	}
	
	err = dom_node_append_child(parser->current, elem, &ins_elem);
	if (err != DOM_NO_ERR) {
		dom_node_unref(elem);
		parser->msg(DOM_MSG_CRITICAL, parser->mctx,
			    "No memory for appending child node");
		return;
	}
	
	dom_node_unref(ins_elem);
	
	dom_node_unref(parser->current);
	parser->current = (struct dom_node *)elem; /* Steal initial ref */
}

static void
expat_xmlparser_end_element_handler(void *_parser,
				    const XML_Char *name)
{
	dom_xml_parser *parser = _parser;
	dom_exception err;
	dom_node *parent;
	
	UNUSED(name);
	
	fprintf(stderr, "</%s>\n", name);
	
	err = dom_node_get_parent_node(parser->current, &parent);
	
	if (err != DOM_NO_ERR) {
		parser->msg(DOM_MSG_CRITICAL, parser->mctx,
			    "Unable to find a parent while closing element.");
		return;
	}
	
	dom_node_unref(parser->current);
	parser->current = parent;  /* Takes the ref given by get_parent_node */
}

static void
expat_xmlparser_cdata_handler(void *_parser,
			      const XML_Char *s,
			      int len)
{
	dom_xml_parser *parser = _parser;
	dom_string *data;
	dom_exception err;
	struct dom_cdata_section *cdata, *ins_cdata;
	
	err = dom_string_create((const uint8_t *)s, len, &data);
	if (err != DOM_NO_ERR) {
		parser->msg(DOM_MSG_CRITICAL, parser->mctx,
			    "No memory for cdata section contents");
		return;
	}
	
	err = dom_document_create_cdata_section(parser->doc, data, &cdata);
	if (err != DOM_NO_ERR) {
		dom_string_unref(data);
		parser->msg(DOM_MSG_CRITICAL, parser->mctx,
			    "No memory for cdata section");
		return;
	}
	
	/* No longer need data */
	dom_string_unref(data);

	/* Append cdata section to parent */
	err = dom_node_append_child(parser->current, (struct dom_node *) cdata,
				    (struct dom_node **) (void *) &ins_cdata);
	if (err != DOM_NO_ERR) {
		dom_node_unref((struct dom_node *) cdata);
		parser->msg(DOM_MSG_ERROR, parser->mctx,
				"Failed attaching cdata section");
		return;
	}

	/* We're not interested in the inserted cdata section */
	if (ins_cdata != NULL)
		dom_node_unref((struct dom_node *) ins_cdata);

	/* No longer interested in cdata section */
	dom_node_unref((struct dom_node *) cdata);
}

static void
expat_xmlparser_unknown_data_handler(void *_parser,
				     const XML_Char *s,
				     int len)
{
	UNUSED(_parser);
	
	fprintf(stderr, "!!! %.*s !!!\n", len, s);
}
/**
 * Create an XML parser instance
 *
 * \param enc      Source charset, or NULL
 * \param int_enc  Desired charset of document buffer (UTF-8 or UTF-16)
 * \param msg      Informational message function
 * \param mctx     Pointer to client-specific private data
 * \return Pointer to instance, or NULL on memory exhaustion
 *
 * int_enc is ignored due to it being made of bees.
 */
dom_xml_parser *
dom_xml_parser_create(const char *enc, const char *int_enc,
		      dom_msg msg, void *mctx)
{
	dom_xml_parser *parser;
	dom_exception err;
	
	UNUSED(int_enc);
	
	parser = calloc(sizeof(*parser), 1);
	if (parser == NULL) {
		msg(DOM_MSG_CRITICAL, mctx, "No memory for parser");
		return NULL;
	}
	
	parser->msg = msg;
	parser->mctx = mctx;
	
	parser->parser = XML_ParserCreateNS(enc, ':');
	
	if (parser->parser == NULL) {
		free(parser);
		msg(DOM_MSG_CRITICAL, mctx, "No memory for parser");
		return NULL;
	}
	
	parser->complete = false;
	parser->doc = NULL;
	
	err = dom_implementation_create_document(
		DOM_IMPLEMENTATION_XML,
		/* namespace */ NULL,
		/* qname */ NULL,
		/* doctype */ NULL,
		NULL,
		&parser->doc);
	
	if (err != DOM_NO_ERR) {
		parser->msg(DOM_MSG_CRITICAL, parser->mctx, 
			    "Failed creating document");
		XML_ParserFree(parser->parser);
		free(parser);
		return NULL;
	}
	
	XML_SetUserData(parser->parser, parser);
	
	XML_SetElementHandler(parser->parser,
			      expat_xmlparser_start_element_handler,
			      expat_xmlparser_end_element_handler);
	
	XML_SetCharacterDataHandler(parser->parser,
				    expat_xmlparser_cdata_handler);
	
	XML_SetParamEntityParsing(parser->parser, 
				  XML_PARAM_ENTITY_PARSING_ALWAYS);
	
	XML_SetDefaultHandler(parser->parser,
			      expat_xmlparser_unknown_data_handler);
	
	parser->current = dom_node_ref(parser->doc);
	
	return parser;
}

/**
 * Destroy an XML parser instance
 *
 * \param parser  The parser instance to destroy
 */
void
dom_xml_parser_destroy(dom_xml_parser *parser)
{
	XML_ParserFree(parser->parser);
	
	free(parser);
}

/**
 * Parse a chunk of data
 *
 * \param parser  The XML parser instance to use for parsing
 * \param data    Pointer to data chunk
 * \param len     Byte length of data chunk
 * \return DOM_XML_OK on success, DOM_XML_EXTERNAL_ERR | <expat error> on failure
 */
dom_xml_error
dom_xml_parser_parse_chunk(dom_xml_parser *parser, uint8_t *data, size_t len)
{
	enum XML_Status status;
	
	status = XML_Parse(parser->parser, (const char *)data, len, 0);
	if (status != XML_STATUS_OK) {
		parser->msg(DOM_MSG_ERROR, parser->mctx,
			    "XML_Parse failed: %d", status);
		return DOM_XML_EXTERNAL_ERR | status;
	}
	
	return DOM_XML_OK;
}

/**
 * Notify parser that datastream is empty
 *
 * \param parser  The XML parser instance to notify
 * \return DOM_XML_OK on success, DOM_XML_EXTERNAL_ERR | <expat error> on failure
 *
 * This will force any remaining data through the parser
 */
dom_xml_error 
dom_xml_parser_completed(dom_xml_parser *parser)
{
	enum XML_Status status;
	
	status = XML_Parse(parser->parser, "", 0, 1);
	if (status != XML_STATUS_OK) {
		parser->msg(DOM_MSG_ERROR, parser->mctx,
			    "XML_Parse failed: %d", status);
		return DOM_XML_EXTERNAL_ERR | status;
	}
	
	parser->complete = true;
	
	return DOM_XML_OK;

}

/**
 * Retrieve the created DOM Document from a parser
 *
 * \param parser  The parser instance to retrieve the document from
 * \return Pointer to document, or NULL if parsing is not complete
 *
 * This may only be called after xml_parser_completed().
 */
struct dom_document *
dom_xml_parser_get_document(dom_xml_parser *parser)
{
	return (parser->complete ? parser->doc : NULL);
}
