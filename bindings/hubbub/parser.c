/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <stdio.h>

#include <hubbub/hubbub.h>
#include <hubbub/parser.h>

#include <dom/dom.h>

#include "parser.h"
#include "utils.h"

/**
 * libdom Hubbub parser object
 */
struct dom_hubbub_parser {
	hubbub_parser *parser;		/**< Hubbub parser instance */
	const uint8_t *buffer;		/**< Parser buffer pointer */

	struct dom_document *doc;	/**< DOM Document we're building */

	bool complete;			/**< Indicate stream completion */

	struct dom_implementation *impl;/**< DOM implementation */

	dom_alloc alloc;		/**< Memory (de)allocation function */
	void *pw;			/**< Pointer to client data */

	dom_msg msg;			/**< Informational messaging function */
	void *mctx;			/**< Pointer to client data */
};

static void __dom_hubbub_buffer_handler(const uint8_t *buffer, size_t len, 
		void *pw);
static void __dom_hubbub_token_handler(const hubbub_token *token, void *pw);

static bool __initialised;

/**
 * Create a Hubbub parser instance
 *
 * \param aliases  Path to encoding alias mapping file
 * \param enc      Source charset, or NULL
 * \param int_enc  Desired charset of document buffer (UTF-8 or UTF-16)
 * \param alloc    Memory (de)allocation function
 * \param pw       Pointer to client-specific private data
 * \param msg      Informational message function
 * \param mctx     Pointer to client-specific private data
 * \return Pointer to instance, or NULL on memory exhaustion
 */
dom_hubbub_parser *dom_hubbub_parser_create(const char *aliases,
		const char *enc, const char *int_enc, 
		dom_alloc alloc, void *pw, dom_msg msg, void *mctx)
{
	dom_hubbub_parser *parser;
	hubbub_parser_optparams params;
	struct dom_string *features;
	dom_exception err;
	hubbub_error e;

	if (__initialised == false) {
		e = hubbub_initialise(aliases, (hubbub_alloc) alloc, pw);
		if (e != HUBBUB_OK) {
			msg(DOM_MSG_ERROR, mctx, 
					"Failed initialising hubbub");
			return NULL;
		}

		__initialised = true;
	}

	parser = alloc(NULL, sizeof(dom_hubbub_parser), pw);
	if (parser == NULL) {
		msg(DOM_MSG_CRITICAL, mctx, "No memory for parser");
		return NULL;
	}

	parser->parser = hubbub_parser_create(enc, int_enc, 
			(hubbub_alloc) alloc, pw);
	if (parser->parser == NULL) {
		alloc(parser, 0, pw);
		msg(DOM_MSG_CRITICAL, mctx, "Failed to create hubbub parser");
		return NULL;
	}

	params.buffer_handler.handler = __dom_hubbub_buffer_handler;
	params.buffer_handler.pw = parser;
	e = hubbub_parser_setopt(parser->parser, HUBBUB_PARSER_BUFFER_HANDLER,
			&params);
	if (e != HUBBUB_OK) {
		hubbub_parser_destroy(parser->parser);
		alloc(parser, 0, pw);
		msg(DOM_MSG_CRITICAL, mctx, 
				"Failed registering hubbub buffer handler");
		return NULL;
	}

	params.token_handler.handler = __dom_hubbub_token_handler;
	params.token_handler.pw = parser;
	e = hubbub_parser_setopt(parser->parser, HUBBUB_PARSER_TOKEN_HANDLER,
			&params);
	if (e != HUBBUB_OK) {
		hubbub_parser_destroy(parser->parser);
		alloc(parser, 0, pw);
		msg(DOM_MSG_CRITICAL, mctx,
				"Failed registering hubbub token handler");
		return NULL;
	}

	parser->doc = NULL;

	parser->complete = false;

	/* Get DOM implementation */
	/* Create string representation of the features we want */
	err = dom_string_create_from_ptr_no_doc(alloc, pw,
			DOM_STRING_UTF8,
			(const uint8_t *) "HTML", SLEN("HTML"), &features);
	if (err != DOM_NO_ERR) {
		hubbub_parser_destroy(parser->parser);
		alloc(parser, 0, pw);
		msg(DOM_MSG_CRITICAL, mctx, "No memory for feature string");
		return NULL;
	}

	/* Now, try to get an appropriate implementation from the registry */
	err = dom_implregistry_get_dom_implementation(features,
			&parser->impl, alloc, pw);
	if (err != DOM_NO_ERR) {
		dom_string_unref(features);
		hubbub_parser_destroy(parser->parser);
		alloc(parser, 0, pw);
		msg(DOM_MSG_ERROR, mctx, "No suitable DOMImplementation");
		return NULL;
	}

	/* no longer need the features string */
	dom_string_unref(features);

	parser->alloc = alloc;
	parser->pw = pw;

	parser->msg = msg;
	parser->mctx = mctx;

	return parser;
}

/* Destroy a Hubbub parser instance */
void dom_hubbub_parser_destroy(dom_hubbub_parser *parser)
{
	dom_implementation_unref(parser->impl);

	hubbub_parser_destroy(parser->parser);

	/** \todo do we want to clean up the document here too? */

	parser->alloc(parser, 0, parser->pw);
}

/* Parse a chunk of data */
dom_hubbub_error dom_hubbub_parser_parse_chunk(dom_hubbub_parser *parser,
		uint8_t *data, size_t len)
{
	hubbub_error err;

	err = hubbub_parser_parse_chunk(parser->parser, data, len);
	if (err != HUBBUB_OK) {
		parser->msg(DOM_MSG_ERROR, parser->mctx,
				"hubbub_parser_parse_chunk failed: %d", err);
		return DOM_HUBBUB_HUBBUB_ERR | err;
	}

	return DOM_HUBBUB_OK;
}

/* Notify parser that datastream is empty */
dom_hubbub_error dom_hubbub_parser_completed(dom_hubbub_parser *parser)
{
	hubbub_error err;

	err = hubbub_parser_completed(parser->parser);
	if (err != DOM_HUBBUB_OK) {
		parser->msg(DOM_MSG_ERROR, parser->mctx,
				"hubbub_parser_completed failed: %d", err);
		return DOM_HUBBUB_HUBBUB_ERR | err;
	}

	parser->complete = true;

	return DOM_HUBBUB_OK;
}

/* Retrieve the created DOM Document */
struct dom_document *dom_hubbub_parser_get_document(dom_hubbub_parser *parser)
{
	return (parser->complete ? parser->doc : NULL);
}

void __dom_hubbub_buffer_handler(const uint8_t *buffer, size_t len, 
		void *pw)
{
	dom_hubbub_parser *parser = (dom_hubbub_parser *) pw;

	UNUSED(len);

	parser->buffer = buffer;
}

void __dom_hubbub_token_handler(const hubbub_token *token, void *pw)
{
	dom_hubbub_parser *parser = (dom_hubbub_parser *) pw;
	static const char *token_names[] = {
		"DOCTYPE", "START TAG", "END TAG",
		"COMMENT", "CHARACTERS", "EOF"
	};
	size_t i;

	printf("%s: ", token_names[token->type]);

	switch (token->type) {
	case HUBBUB_TOKEN_DOCTYPE:
		printf("'%.*s' (%svalid)\n",
				(int) token->data.doctype.name.len,
				parser->buffer + 
					token->data.doctype.name.data_off,
				token->data.doctype.correct ? "" : "in");
		break;
	case HUBBUB_TOKEN_START_TAG:
		printf("'%.*s' %s\n",
				(int) token->data.tag.name.len,
				parser->buffer + token->data.tag.name.data_off,
				(token->data.tag.n_attributes > 0) ?
						"attributes:" : "");
		for (i = 0; i < token->data.tag.n_attributes; i++) {
			printf("\t'%.*s' = '%.*s'\n",
					(int) token->data.tag.attributes[i].name.len,
					parser->buffer + token->data.tag.attributes[i].name.data_off,
					(int) token->data.tag.attributes[i].value.len,
					parser->buffer + token->data.tag.attributes[i].value.data_off);
		}
		break;
	case HUBBUB_TOKEN_END_TAG:
		printf("'%.*s' %s\n",
				(int) token->data.tag.name.len,
				parser->buffer + token->data.tag.name.data_off,
				(token->data.tag.n_attributes > 0) ?
						"attributes:" : "");
		for (i = 0; i < token->data.tag.n_attributes; i++) {
			printf("\t'%.*s' = '%.*s'\n",
					(int) token->data.tag.attributes[i].name.len,
					parser->buffer + token->data.tag.attributes[i].name.data_off,
					(int) token->data.tag.attributes[i].value.len,
					parser->buffer + token->data.tag.attributes[i].value.data_off);
		}
		break;
	case HUBBUB_TOKEN_COMMENT:
		printf("'%.*s'\n", (int) token->data.comment.len,
				parser->buffer + token->data.comment.data_off);
		break;
	case HUBBUB_TOKEN_CHARACTER:
		printf("'%.*s'\n", (int) token->data.character.len,
				parser->buffer + token->data.character.data_off);
		break;
	case HUBBUB_TOKEN_EOF:
		printf("\n");
		break;
	}
}

