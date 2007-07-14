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
static xmlSAXHandler sax_handler;
#if 0
 = {
    internalSubsetSAXFunc internalSubset;
    isStandaloneSAXFunc isStandalone;
    hasInternalSubsetSAXFunc hasInternalSubset;
    hasExternalSubsetSAXFunc hasExternalSubset;
    resolveEntitySAXFunc resolveEntity;
    getEntitySAXFunc getEntity;
    entityDeclSAXFunc entityDecl;
    notationDeclSAXFunc notationDecl;
    attributeDeclSAXFunc attributeDecl;
    elementDeclSAXFunc elementDecl;
    unparsedEntityDeclSAXFunc unparsedEntityDecl;
    setDocumentLocatorSAXFunc setDocumentLocator;
    startDocumentSAXFunc startDocument;
    endDocumentSAXFunc endDocument;
    startElementSAXFunc startElement;
    endElementSAXFunc endElement;
    referenceSAXFunc reference;
    charactersSAXFunc characters;
    ignorableWhitespaceSAXFunc ignorableWhitespace;
    processingInstructionSAXFunc processingInstruction;
    commentSAXFunc comment;
    warningSAXFunc warning;
    errorSAXFunc error;
    fatalErrorSAXFunc fatalError; /* unused error() get all the errors */
    getParameterEntitySAXFunc getParameterEntity;
    cdataBlockSAXFunc cdataBlock;
    externalSubsetSAXFunc externalSubset;
    unsigned int initialized;
    /* The following fields are extensions available only on version 2 */
    void *_private;
    startElementNsSAX2Func startElementNs;
    endElementNsSAX2Func endElementNs;
    xmlStructuredErrorFunc serror;
};
#endif

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
