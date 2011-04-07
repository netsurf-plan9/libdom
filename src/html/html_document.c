/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>

#include "html/html_document.h"

#include "core/string.h"
#include "utils/utils.h"

/* Create a HTMLDocument */
dom_exception dom_html_document_create(dom_alloc alloc, void *pw, dom_msg msg,
		void *msg_pw,
		dom_events_default_action_fetcher daf, dom_ui_handler *ui,
		dom_parser_type pt, dom_html_document **doc)
{
	assert(alloc != NULL);
	*doc = alloc(NULL, sizeof(dom_html_document), pw);
	if (*doc == NULL)
		return DOM_NO_MEM_ERR;
	
	return _dom_html_document_initialise(*doc, alloc, pw, msg, msg_pw,
			daf, ui, pt);
}

/* Initialise a HTMLDocument */
dom_exception _dom_html_document_initialise(dom_html_document *doc,
		dom_alloc alloc, void *pw, dom_msg msg, void *msg_pw,
		dom_events_default_action_fetcher daf, dom_ui_handler *ui,
		dom_parser_type pt)
{
	UNUSED(doc);
	UNUSED(alloc);
	UNUSED(pw);
	UNUSED(msg);
	UNUSED(msg_pw);
	UNUSED(daf);
	UNUSED(ui);
	UNUSED(pt);

	return DOM_NO_ERR;
}

/* Finalise a HTMLDocument */
void _dom_html_document_finalise(dom_html_document *doc);
/* Destroy a HTMLDocument */
void _dom_html_document_destroy(dom_html_document *doc);

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

