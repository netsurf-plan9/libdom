/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <stdbool.h>

#include <libxml/parser.h>
#include <libxml/SAX2.h>
#include <libxml/xmlerror.h>

#include <dom/dom.h>

#include "xmlerror.h"
#include "xmlparser.h"
#include "utils.h"

static void xml_parser_start_document(void *ctx);
static void xml_parser_start_element_ns(void *ctx, const xmlChar *localname,
		const xmlChar *prefix, const xmlChar *URI,
		int nb_namespaces, const xmlChar **namespaces,
		int nb_attributes, int nb_defaulted,
		const xmlChar **attributes);

/**
 * XML parser object
 */
struct xml_parser {
	xmlParserCtxtPtr xml_ctx;	/**< libxml parser context */

	struct dom_document *doc;	/**< DOM Document we're building */

	bool complete;			/**< Indicate stream completion */

	xml_alloc alloc;		/**< Memory (de)allocation function */
	void *pw;			/**< Pointer to client data */
};

/**
 * SAX callback dispatch table
 */
static xmlSAXHandler sax_handler = {
	.internalSubset = xmlSAX2InternalSubset,
	.isStandalone = xmlSAX2IsStandalone,
	.hasInternalSubset = xmlSAX2HasInternalSubset,
	.hasExternalSubset = xmlSAX2HasExternalSubset,
	.resolveEntity = xmlSAX2ResolveEntity,
	.getEntity = xmlSAX2GetEntity,
	.entityDecl = xmlSAX2EntityDecl,
	.notationDecl = xmlSAX2NotationDecl,
	.attributeDecl = xmlSAX2AttributeDecl,
	.elementDecl = xmlSAX2ElementDecl,
	.unparsedEntityDecl = xmlSAX2UnparsedEntityDecl,
	.setDocumentLocator = xmlSAX2SetDocumentLocator,
	.startDocument = xml_parser_start_document,
	.endDocument = xmlSAX2EndDocument,
	.startElement = NULL,
	.endElement = NULL,
	.reference = xmlSAX2Reference,
	.characters = xmlSAX2Characters,
	.ignorableWhitespace = xmlSAX2Characters,
	.processingInstruction = NULL,
	.comment = xmlSAX2Comment,
	.warning = xmlParserWarning,
	.error = xmlParserError,
	.fatalError = xmlParserError,
	.getParameterEntity = xmlSAX2GetParameterEntity,
	.cdataBlock = xmlSAX2CDataBlock,
	.externalSubset = xmlSAX2ExternalSubset,
	.initialized = XML_SAX2_MAGIC,
	._private = NULL,
	.startElementNs = xml_parser_start_element_ns,
	.endElementNs = xmlSAX2EndElementNs,
	.serror = NULL,
};

/**
 * Create an XML parser instance
 *
 * \param enc      Source charset, or NULL
 * \param int_enc  Desired charset of document buffer (UTF-8 or UTF-16)
 * \param alloc    Memory (de)allocation function
 * \param pw       Pointer to client-specific private data
 * \return Pointer to instance, or NULL on memory exhaustion
 *
 * Neither ::enc nor ::int_enc are used here.
 * libxml only supports a UTF-8 document buffer and forcibly setting the
 * parser encoding is not yet implemented
 */
xml_parser *xml_parser_create(const char *enc, const char *int_enc,
		xml_alloc alloc, void *pw)
{
	xml_parser *parser;

	UNUSED(enc);
	UNUSED(int_enc);

	parser = alloc(NULL, sizeof(xml_parser), pw);
	if (parser == NULL)
		return NULL;

	parser->xml_ctx =
		xmlCreatePushParserCtxt(&sax_handler, parser, "", 0,
				NULL);
	if (parser->xml_ctx == NULL) {
		alloc(parser, 0, pw);
		return NULL;
	}

	parser->doc = NULL;

	parser->complete = false;

	parser->alloc = alloc;
	parser->pw = pw;

	return parser;
}

/**
 * Destroy an XML parser instance
 *
 * \param parser  The parser instance to destroy
 */
void xml_parser_destroy(xml_parser *parser)
{
	xmlFreeParserCtxt(parser->xml_ctx);

	/** \todo Do we want to clean up the document here, too? */
	/* Obviously, document cleanup wouldn't happen if the client has
	 * claimed the document from us via xml_parser_get_document() */

	parser->alloc(parser, 0, parser->pw);
}

/**
 * Parse a chunk of data
 *
 * \param parser  The XML parser instance to use for parsing
 * \param data    Pointer to data chunk
 * \param len     Byte length of data chunk
 * \return XML_OK on success, XML_LIBXML_ERR | <libxml error> on failure
 */
xml_error xml_parser_parse_chunk(xml_parser *parser,
		uint8_t *data, size_t len)
{
	xmlParserErrors err;

	err = xmlParseChunk(parser->xml_ctx, (char *) data, len, 0);
	if (err != XML_ERR_OK)
		return XML_LIBXML_ERR | err;

	return XML_OK;
}

/**
 * Notify parser that datastream is empty
 *
 * \param parser  The XML parser instance to notify
 * \return XML_OK on success, XML_LIBXML_ERR | <libxml error> on failure
 *
 * This will force any remaining data through the parser
 */
xml_error xml_parser_completed(xml_parser *parser)
{
	xmlParserErrors err;

	err = xmlParseChunk(parser->xml_ctx, "", 0, 1);
	if (err != XML_ERR_OK)
		return XML_LIBXML_ERR | err;

	parser->complete = true;

	return XML_OK;
}

/**
 * Retrieve the created DOM Document from a parser
 *
 * \param parser  The parser instance to retrieve the document from
 * \return Pointer to document, or NULL if parsing is not complete
 *
 * This may only be called after xml_parser_completed().
 */
struct dom_document *xml_parser_get_document(xml_parser *parser)
{
	return (parser->complete ? parser->doc : NULL);
}

/**
 * Handle a document start SAX event
 *
 * \param ctx  The callback context
 */
void xml_parser_start_document(void *ctx)
{
	xml_parser *parser = (xml_parser *) ctx;
	struct dom_implementation *impl;
	struct dom_string *features, *udkey;
	struct dom_document *doc;
	void *ignored;
	dom_exception err;

	/* Invoke libxml2's default behaviour */
	xmlSAX2StartDocument(parser->xml_ctx);

	/* Create a string representation of the features we want */
	err = dom_string_create_from_ptr_no_doc((dom_alloc) parser->alloc,
			parser->pw, (const uint8_t *) "XML", SLEN("XML"),
			&features);
	if (err != DOM_NO_ERR)
		return;

	/* Now, try to get an appropriate implementation from the registry */
	err = dom_implregistry_get_dom_implementation(features, &impl,
			(dom_alloc) parser->alloc, parser->pw);
	if (err != DOM_NO_ERR) {
		dom_string_unref(features);
		return;
	}

	/* No longer need the features string */
	dom_string_unref(features);

	/* Attempt to create a document */
	err = dom_implementation_create_document(impl, /* namespace */ NULL,
			/* qname */ NULL, /* doctype */ NULL,
			&doc, (dom_alloc) parser->alloc, parser->pw);
	if (err != DOM_NO_ERR) {
		dom_implementation_unref(impl);
		return;
	}

	/* No longer need the implementation */
	dom_implementation_unref(impl);

	/* Create key for user data registration */
	err = dom_string_create_from_const_ptr(doc,
			(const uint8_t *) "__xmlnode", SLEN("__xmlnode"),
			&udkey);
	if (err != DOM_NO_ERR)
		return;

	/* Register xmlNode as userdata for document */
	err = dom_node_set_user_data((struct dom_node *) doc,
			udkey, parser->xml_ctx->myDoc, NULL, &ignored);
	if (err != DOM_NO_ERR) {
		dom_string_unref(udkey);
		return;
	}

	/* No longer need the key */
	dom_string_unref(udkey);

	/* Register the DOM node with the xmlNode */
	dom_node_ref((struct dom_node *) doc);
	parser->xml_ctx->myDoc->_private = doc;

	/* And squirrel the document away for later use */
	parser->doc = doc;
}

/**
 * Handle an element open SAX event
 *
 * \param ctx            The callback context
 * \param localname      The local name of the element
 * \param prefix         The element namespace prefix
 * \param URI            The element namespace URI
 * \param nb_namespaces  The number of namespace definitions
 * \param namespaces     Array of nb_namespaces prefix/URI pairs
 * \param nb_attributes  The number of attributes
 * \param nb_defaulted   The number of defaulted attributes
 * \param attributes     Array of [nb_attributes + nb_defaulted] attribute
 *                       values
 */
void xml_parser_start_element_ns(void *ctx, const xmlChar *localname,
		const xmlChar *prefix, const xmlChar *URI,
		int nb_namespaces, const xmlChar **namespaces,
		int nb_attributes, int nb_defaulted,
		const xmlChar **attributes)
{
	xml_parser *parser = (xml_parser *) ctx;

	/* Invoke libxml2's default behaviour */
	xmlSAX2StartElementNs(parser->xml_ctx, localname, prefix, URI,
			nb_namespaces, namespaces, nb_attributes,
			nb_defaulted, attributes);

	/** \todo mirror the xml tree in the DOM */
}

