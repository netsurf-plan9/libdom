/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_html_document_h_
#define dom_html_document_h_

#include <dom/core/exceptions.h>
#include <dom/functypes.h>
#include <dom/events/document_event.h>

struct dom_element;
struct dom_html_collection;
struct dom_html_element;
struct dom_nodelist;

typedef struct dom_html_document dom_html_document;

typedef enum {
	DOM_HTML_PARSER = 0,
	DOM_XML_PARSER  = 1
} dom_parser_type;

/**
 * Callbacks for UI related function calls
 */
struct dom_ui_handler {
	dom_exception (*element_focus)(struct dom_element *element);
			/* Callback for a focus DOM call */
	dom_exception (*element_select)(struct dom_element *element);
			/* Callback for a select DOM call */
	dom_exception (*element_blur)(struct dom_element *element);
			/* Callback for a blur DOM call */
	dom_exception (*element_enable)(struct dom_element *element,
			bool enable);
			/* Callback for enable/disable an element */
};
typedef struct dom_ui_handler dom_ui_handler;

/* Create a HTMLDocument */
dom_exception dom_html_document_create(dom_alloc alloc, void *pw, dom_msg msg,
		void *msg_pw,
		dom_events_default_action_fetcher daf, dom_ui_handler *ui,
		dom_parser_type pt, dom_html_document **doc);

/* Parse a data chunk into the HTMLDocument */
dom_exception dom_html_document_write_data(uint8_t *data, size_t len);

/* Notify the HTMLDocument that it is completed */
dom_exception dom_html_document_complete(void);

/*-----------------------------------------------------------------------*/
/* The DOM spec public API */

dom_exception dom_html_document_get_title(dom_html_document *doc,
		struct dom_string **title);
dom_exception dom_html_document_set_title(dom_html_document *doc,
		struct dom_string *title);
dom_exception dom_html_document_get_referer(dom_html_document *doc,
		struct dom_string **referer);
dom_exception dom_html_document_get_domain(dom_html_document *doc,
		struct dom_string **domain);
dom_exception dom_html_document_get_url(dom_html_document *doc,
		struct dom_string **url);
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
		struct dom_string **cookie);
dom_exception dom_html_document_set_cookie(dom_html_document *doc,
		struct dom_string *cookie);

dom_exception dom_html_document_open(dom_html_document *doc);
dom_exception dom_html_document_close(dom_html_document *doc);
dom_exception dom_html_document_write(dom_html_document *doc,
		struct dom_string *text);
dom_exception dom_html_document_writeln(dom_html_document *doc,
		struct dom_string *text);
dom_exception dom_html_document_get_elements_by_name(dom_html_document *doc,
		struct dom_string *name, struct dom_nodelist **list);

#endif

