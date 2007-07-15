/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <stdbool.h>
#include <stdio.h>

#include <libxml/parser.h>
#include <libxml/SAX2.h>
#include <libxml/xmlerror.h>

#include <dom/dom.h>

#include "xmlerror.h"
#include "xmlparser.h"
#include "utils.h"

static void xml_parser_start_document(void *ctx);
static void xml_parser_end_document(void *ctx);
static void xml_parser_start_element_ns(void *ctx, const xmlChar *localname,
		const xmlChar *prefix, const xmlChar *URI,
		int nb_namespaces, const xmlChar **namespaces,
		int nb_attributes, int nb_defaulted,
		const xmlChar **attributes);
static void xml_parser_end_element_ns(void *ctx, const xmlChar *localname,
		const xmlChar *prefix, const xmlChar *URI);

static void xml_parser_add_node(xml_parser *parser, struct dom_node *parent,
		xmlNodePtr child);

static void xml_parser_internal_subset(void *ctx, const xmlChar *name,
		const xmlChar *ExternalID, const xmlChar *SystemID);
static int xml_parser_is_standalone(void *ctx);
static int xml_parser_has_internal_subset(void *ctx);
static int xml_parser_has_external_subset(void *ctx);
static xmlParserInputPtr xml_parser_resolve_entity(void *ctx,
		const xmlChar *publicId, const xmlChar *systemId);
static xmlEntityPtr xml_parser_get_entity(void *ctx, const xmlChar *name);
static void xml_parser_entity_decl(void *ctx, const xmlChar *name,
		int type, const xmlChar *publicId, const xmlChar *systemId,
		xmlChar *content);
static void xml_parser_notation_decl(void *ctx, const xmlChar *name,
		const xmlChar *publicId, const xmlChar *systemId);
static void xml_parser_attribute_decl(void *ctx, const xmlChar *elem,
		const xmlChar *fullname, int type, int def,
		const xmlChar *defaultValue, xmlEnumerationPtr tree);
static void xml_parser_element_decl(void *ctx, const xmlChar *name,
		int type, xmlElementContentPtr content);
static void xml_parser_unparsed_entity_decl(void *ctx, const xmlChar *name,
		const xmlChar *publicId, const xmlChar *systemId,
		const xmlChar *notationName);
static void xml_parser_set_document_locator(void *ctx, xmlSAXLocatorPtr loc);
static void xml_parser_reference(void *ctx, const xmlChar *name);
static void xml_parser_characters(void *ctx, const xmlChar *ch, int len);
static void xml_parser_comment(void *ctx, const xmlChar *value);
static xmlEntityPtr xml_parser_get_parameter_entity(void *ctx,
		const xmlChar *name);
static void xml_parser_cdata_block(void *ctx, const xmlChar *value, int len);
static void xml_parser_external_subset(void *ctx, const xmlChar *name,
		const xmlChar *ExternalID, const xmlChar *SystemID);

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
	.internalSubset = xml_parser_internal_subset,
	.isStandalone = xml_parser_is_standalone,
	.hasInternalSubset = xml_parser_has_internal_subset,
	.hasExternalSubset = xml_parser_has_external_subset,
	.resolveEntity = xml_parser_resolve_entity,
	.getEntity = xml_parser_get_entity,
	.entityDecl = xml_parser_entity_decl,
	.notationDecl = xml_parser_notation_decl,
	.attributeDecl = xml_parser_attribute_decl,
	.elementDecl = xml_parser_element_decl,
	.unparsedEntityDecl = xml_parser_unparsed_entity_decl,
	.setDocumentLocator = xml_parser_set_document_locator,
	.startDocument = xml_parser_start_document,
	.endDocument = xml_parser_end_document,
	.startElement = NULL,
	.endElement = NULL,
	.reference = xml_parser_reference,
	.characters = xml_parser_characters,
	.ignorableWhitespace = xml_parser_characters,
	.processingInstruction = NULL,
	.comment = xml_parser_comment,
	.warning = NULL,
	.error = NULL,
	.fatalError = NULL,
	.getParameterEntity = xml_parser_get_parameter_entity,
	.cdataBlock = xml_parser_cdata_block,
	.externalSubset = xml_parser_external_subset,
	.initialized = XML_SAX2_MAGIC,
	._private = NULL,
	.startElementNs = xml_parser_start_element_ns,
	.endElementNs = xml_parser_end_element_ns,
	.serror = NULL
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
		xmlCreatePushParserCtxt(&sax_handler, parser, "", 0, NULL);
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
 * Handle a document end SAX event
 *
 * \param ctx  The callback context
 */
void xml_parser_end_document(void *ctx)
{
	xml_parser *parser = (xml_parser *) ctx;
	struct dom_string *key;
	xmlNodePtr node;
	xmlNodePtr n;
	dom_exception err;

	/* Invoke libxml2's default behaviour */
	xmlSAX2EndDocument(parser->xml_ctx);

	/* If there is no document, we can't do anything */
	if (parser->doc == NULL)
		return;

	/* We need to mirror any child nodes at the end of the list of
	 * children which occur after the last Element node in the list */

	/* Create key */
	err = dom_string_create_from_const_ptr(parser->doc,
			(const uint8_t *) "__xmlnode", SLEN("__xmlnode"),
			&key);
	if (err != DOM_NO_ERR)
		return;

	/* Get XML node */
	err = dom_node_get_user_data((struct dom_node *) parser->doc, key,
			(void **) &node);
	if (err != DOM_NO_ERR) {
		dom_string_unref(key);
		return;
	}

	/* No longer need key */
	dom_string_unref(key);

	/* Find last Element node, if any */
	for (n = node->last; n != NULL; n = n->prev) {
		if (n->type == XML_ELEMENT_NODE)
			break;
	}

	if (n == NULL) {
		/* No Element node found; entire list needs mirroring */
		n = node->children;
	} else {
		/* Found last Element; skip over it */
		n = n->next;
	}

	/* Now, mirror nodes in the DOM */
	for (; n != NULL; n = n->next) {
		xml_parser_add_node(parser,
				(struct dom_node *) node->_private, n);
	}
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
 * \param nb_attributes  The total number of attributes
 * \param nb_defaulted   The number of defaulted attributes
 * \param attributes     Array of nb_attributes attribute values
 *
 * The number of non-defaulted attributes is ::nb_attributes - ::nb_defaulted
 * The defaulted attributes are at the end of the array ::attributes.
 */
void xml_parser_start_element_ns(void *ctx, const xmlChar *localname,
		const xmlChar *prefix, const xmlChar *URI,
		int nb_namespaces, const xmlChar **namespaces,
		int nb_attributes, int nb_defaulted,
		const xmlChar **attributes)
{
	xml_parser *parser = (xml_parser *) ctx;
	xmlNodePtr parent = parser->xml_ctx->node;

	/* Invoke libxml2's default behaviour */
	xmlSAX2StartElementNs(parser->xml_ctx, localname, prefix, URI,
			nb_namespaces, namespaces, nb_attributes,
			nb_defaulted, attributes);

	/* If there is no document, we can't do anything */
	if (parser->doc == NULL)
		return;

	if (parent == NULL) {
		/* No parent; use document */
		parent = (xmlNodePtr) parser->xml_ctx->myDoc;
	}

	if (parent->type == XML_DOCUMENT_NODE ||
			parent->type == XML_ELEMENT_NODE) {
		/* Mirror in the DOM all children of the parent node
		 * between the previous Element child (or the start,
		 * whichever is encountered first) and the Element
		 * just created */
		xmlNodePtr n;

		/* Find previous element node, if any */
		for (n = parser->xml_ctx->node->prev; n != NULL;
				n = n->prev) {
			if (n->type == XML_ELEMENT_NODE)
				break;
		}

		if (n == NULL) {
			/* No previous Element; use parent's children */
			n = parent->children;
		} else {
			/* Previous Element; skip over it */
			n = n->next;
		}

		/* Now, mirror nodes in the DOM */
		for (; n != parser->xml_ctx->node; n = n->next) {
			xml_parser_add_node(parser,
					(struct dom_node *) parent->_private,
					n);
		}
	}

	/* Mirror the created node and its attributes in the DOM */
	xml_parser_add_node(parser, (struct dom_node *) parent->_private,
			parser->xml_ctx->node);

}

/**
 * Handle an element close SAX event
 *
 * \param ctx        The callback context
 * \param localname  The local name of the element
 * \param prefix     The element namespace prefix
 * \param URI        The element namespace URI
 */
void xml_parser_end_element_ns(void *ctx, const xmlChar *localname,
		const xmlChar *prefix, const xmlChar *URI)
{
	xml_parser *parser = (xml_parser *) ctx;
	xmlNodePtr node = parser->xml_ctx->node;
	xmlNodePtr n;

	/* Invoke libxml2's default behaviour */
	xmlSAX2EndElementNs(parser->xml_ctx, localname, prefix, URI);

	/* If there is no document, we can't do anything */
	if (parser->doc == NULL)
		return;

	/* We need to mirror any child nodes at the end of the list of
	 * children which occur after the last Element node in the list */

	/* Find last Element node, if any */
	for (n = node->last; n != NULL; n = n->prev) {
		if (n->type == XML_ELEMENT_NODE)
			break;
	}

	if (n == NULL) {
		/* No Element node found; entire list needs mirroring */
		n = node->children;
	} else {
		/* Found last Element; skip over it */
		n = n->next;
	}

	/* Now, mirror nodes in the DOM */
	for (; n != NULL; n = n->next) {
		xml_parser_add_node(parser,
				(struct dom_node *) node->_private, n);
	}
}

/**
 * Add a node to the DOM
 *
 * \param parser  The parser context
 * \param parent  The parent DOM node
 * \param child   The xmlNode to mirror in the DOM as a child of parent
 */
void xml_parser_add_node(xml_parser *parser, struct dom_node *parent,
		xmlNodePtr child)
{
	UNUSED(parser);
	UNUSED(parent);

	switch (child->type) {
	case XML_ELEMENT_NODE:
	case XML_TEXT_NODE:
	case XML_CDATA_SECTION_NODE:
	case XML_PI_NODE:
	case XML_COMMENT_NODE:
	case XML_DOCUMENT_NODE:
	case XML_DOCUMENT_TYPE_NODE:
	case XML_NOTATION_NODE:
	case XML_DTD_NODE:
	default:
		fprintf(stderr, "Unsupported node type: %d\n", child->type);
	}
}

/*                                                                         */
/* ------------------------------------------------------------------------*/
/*                                                                         */
void xml_parser_internal_subset(void *ctx, const xmlChar *name,
		const xmlChar *ExternalID, const xmlChar *SystemID)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2InternalSubset(parser->xml_ctx, name, ExternalID, SystemID);
}

int xml_parser_is_standalone(void *ctx)
{
	xml_parser *parser = (xml_parser *) ctx;

	return xmlSAX2IsStandalone(parser->xml_ctx);
}

int xml_parser_has_internal_subset(void *ctx)
{
	xml_parser *parser = (xml_parser *) ctx;

	return xmlSAX2HasInternalSubset(parser->xml_ctx);
}

int xml_parser_has_external_subset(void *ctx)
{
	xml_parser *parser = (xml_parser *) ctx;

	return xmlSAX2HasExternalSubset(parser->xml_ctx);
}

xmlParserInputPtr xml_parser_resolve_entity(void *ctx,
		const xmlChar *publicId, const xmlChar *systemId)
{
	xml_parser *parser = (xml_parser *) ctx;

	return xmlSAX2ResolveEntity(parser->xml_ctx, publicId, systemId);
}

xmlEntityPtr xml_parser_get_entity(void *ctx, const xmlChar *name)
{
	xml_parser *parser = (xml_parser *) ctx;

	return xmlSAX2GetEntity(parser->xml_ctx, name);
}

void xml_parser_entity_decl(void *ctx, const xmlChar *name,
		int type, const xmlChar *publicId, const xmlChar *systemId,
		xmlChar *content)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2EntityDecl(parser->xml_ctx, name, type, publicId, systemId,
			content);
}

void xml_parser_notation_decl(void *ctx, const xmlChar *name,
		const xmlChar *publicId, const xmlChar *systemId)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2NotationDecl(parser->xml_ctx, name, publicId, systemId);
}

void xml_parser_attribute_decl(void *ctx, const xmlChar *elem,
		const xmlChar *fullname, int type, int def,
		const xmlChar *defaultValue, xmlEnumerationPtr tree)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2AttributeDecl(parser->xml_ctx, elem, fullname, type, def,
			defaultValue, tree);
}

void xml_parser_element_decl(void *ctx, const xmlChar *name,
		int type, xmlElementContentPtr content)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2ElementDecl(parser->xml_ctx, name, type, content);
}

void xml_parser_unparsed_entity_decl(void *ctx, const xmlChar *name,
		const xmlChar *publicId, const xmlChar *systemId,
		const xmlChar *notationName)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2UnparsedEntityDecl(parser->xml_ctx, name, publicId,
			systemId, notationName);
}

void xml_parser_set_document_locator(void *ctx, xmlSAXLocatorPtr loc)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2SetDocumentLocator(parser->xml_ctx, loc);
}

void xml_parser_reference(void *ctx, const xmlChar *name)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2Reference(parser->xml_ctx, name);
}

void xml_parser_characters(void *ctx, const xmlChar *ch, int len)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2Characters(parser->xml_ctx, ch, len);
}

void xml_parser_comment(void *ctx, const xmlChar *value)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2Comment(parser->xml_ctx, value);
}

xmlEntityPtr xml_parser_get_parameter_entity(void *ctx, const xmlChar *name)
{
	xml_parser *parser = (xml_parser *) ctx;

	return xmlSAX2GetParameterEntity(parser->xml_ctx, name);
}

void xml_parser_cdata_block(void *ctx, const xmlChar *value, int len)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2CDataBlock(parser->xml_ctx, value, len);
}

void xml_parser_external_subset(void *ctx, const xmlChar *name,
		const xmlChar *ExternalID, const xmlChar *SystemID)
{
	xml_parser *parser = (xml_parser *) ctx;

	xmlSAX2ExternalSubset(parser->xml_ctx, name, ExternalID, SystemID);
}
