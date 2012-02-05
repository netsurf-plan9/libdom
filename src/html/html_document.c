/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>

#include "html/html_document.h"

#include "core/string.h"
#include "utils/utils.h"

/* Create a HTMLDocument */
dom_exception dom_html_document_create(
		dom_events_default_action_fetcher daf, dom_ui_handler *ui,
		dom_html_document **doc)
{
	dom_exception error;
	dom_html_document *result;

	result = malloc(sizeof(dom_html_document));
	if (result == NULL)
		return DOM_NO_MEM_ERR;
	
	error = _dom_html_document_initialise(*doc, daf, ui);
	if (error != DOM_NO_ERR) {
		free(result);
		return error;
	}

	*doc = result;
	return DOM_NO_ERR;
}

/* Initialise a HTMLDocument */
dom_exception _dom_html_document_initialise(dom_html_document *doc,
		dom_events_default_action_fetcher daf, dom_ui_handler *ui)
{
	dom_exception error;

	UNUSED(ui);

	error = _dom_document_initialise(&doc->base, daf);
	if (error != DOM_NO_ERR)
		return error;

	doc->title = NULL;
	doc->referer = NULL;
	doc->domain = NULL;
	doc->url = NULL;
	doc->cookie = NULL;

	return DOM_NO_ERR;
}

/* Finalise a HTMLDocument */
void _dom_html_document_finalise(dom_html_document *doc)
{
	dom_string_unref(doc->cookie);
	dom_string_unref(doc->url);
	dom_string_unref(doc->domain);
	dom_string_unref(doc->referer);
	dom_string_unref(doc->title);

	_dom_document_finalise(&doc->base);
}

/* Destroy a HTMLDocument */
void _dom_html_document_destroy(dom_html_document *doc)
{
	_dom_html_document_finalise(doc);

	free(doc);
}

/* Parse a data chunk into the HTMLDocument */
dom_exception dom_html_document_write_data(uint8_t *data, size_t len);

/* Notify the HTMLDocument that it is completed */
dom_exception dom_html_document_complete(void);

/*-----------------------------------------------------------------------*/
/* The DOM spec public API */

/**
 * Get the title of this HTMLDocument 
 * \param doc    The document object
 * \param title  The reutrned title string
 * \return DOM_NO_ERR on success, appropriated dom_exception on failure.
 *
 * @note: this method find a title for the document as following:
 * 1. If there is a title in the document object set by 
 *    dom_html_document_set_title, then use it;
 * 2. If there is no such one, find the <title> element and use its text
 *    as the returned title.
 */
dom_exception dom_html_document_get_title(dom_html_document *doc,
		dom_string **title)
{
	UNUSED(doc);
	UNUSED(title);

	return DOM_NO_ERR;
}

dom_exception dom_html_document_set_title(dom_html_document *doc,
		dom_string *title);
dom_exception dom_html_document_get_referer(dom_html_document *doc,
		dom_string **referer);
dom_exception dom_html_document_get_domain(dom_html_document *doc,
		dom_string **domain);
dom_exception dom_html_document_get_url(dom_html_document *doc,
		dom_string **url);
dom_exception dom_html_document_get_body(dom_html_document *doc,
		struct dom_html_element **body);
dom_exception dom_html_document_set_body(dom_html_document *doc,
		struct dom_html_element *body);
dom_exception dom_html_document_get_images(dom_html_document *doc,
		struct dom_html_collection **col);
dom_exception dom_html_document_get_applets(dom_html_document *doc,
		struct dom_html_collection **col);
dom_exception dom_html_document_get_links(dom_html_document *doc,
		struct dom_html_collection **col);
dom_exception dom_html_document_get_forms(dom_html_document *doc,
		struct dom_html_collection **col);
dom_exception dom_html_document_get_anchors(dom_html_document *doc,
		struct dom_html_collection **col);
dom_exception dom_html_document_get_cookie(dom_html_document *doc,
		dom_string **cookie);
dom_exception dom_html_document_set_cookie(dom_html_document *doc,
		dom_string *cookie);

dom_exception dom_html_document_open(dom_html_document *doc);
dom_exception dom_html_document_close(dom_html_document *doc);
dom_exception dom_html_document_write(dom_html_document *doc,
		dom_string *text);
dom_exception dom_html_document_writeln(dom_html_document *doc,
		dom_string *text);
dom_exception dom_html_document_get_elements_by_name(dom_html_document *doc,
		dom_string *name, struct dom_nodelist **list);

