/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>

#include "html/html_document.h"
#include "html/html_element.h"

#include "core/string.h"
#include "utils/namespace.h"
#include "utils/utils.h"
#include "utils/validate.h"

static struct dom_html_document_vtable html_document_vtable = {
	{
		{
			{
				DOM_NODE_EVENT_TARGET_VTABLE
			},
			DOM_NODE_VTABLE,
		},
		DOM_DOCUMENT_VTABLE_HTML
	},
	DOM_HTML_DOCUMENT_VTABLE
};

static struct dom_node_protect_vtable html_document_protect_vtable = {
	DOM_HTML_DOCUMENT_PROTECT_VTABLE
};

/* Create a HTMLDocument */
dom_exception _dom_html_document_create(
		dom_events_default_action_fetcher daf,
		dom_html_document **doc)
{
	dom_exception error;
	dom_html_document *result;

	result = malloc(sizeof(dom_html_document));
	if (result == NULL)
		return DOM_NO_MEM_ERR;

	result->base.base.base.vtable = &html_document_vtable;
	result->base.base.vtable = &html_document_protect_vtable;
	
	error = _dom_html_document_initialise(result, daf);
	if (error != DOM_NO_ERR) {
		free(result);
		return error;
	}

	*doc = result;
	return DOM_NO_ERR;
}

/* Initialise a HTMLDocument */
dom_exception _dom_html_document_initialise(dom_html_document *doc,
		dom_events_default_action_fetcher daf)
{
	dom_exception error;

	error = _dom_document_initialise(&doc->base, daf);
	if (error != DOM_NO_ERR)
		return error;

	doc->title = NULL;
	doc->referrer = NULL;
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
	dom_string_unref(doc->referrer);
	dom_string_unref(doc->title);

	_dom_document_finalise(&doc->base);
}

/* Destroy a HTMLDocument */
void _dom_html_document_destroy(dom_node_internal *node)
{
	dom_html_document *doc = (dom_html_document *) node;

	_dom_html_document_finalise(doc);

	free(doc);
}

dom_exception _dom_html_document_copy(dom_node_internal *old,
		dom_node_internal **copy)
{
	UNUSED(old);
	UNUSED(copy);

	return DOM_NOT_SUPPORTED_ERR;
}

/* Overloaded methods inherited from super class */
/** \todo: dispatch on tag name to create correct HTMLElement subclass */

dom_exception _dom_html_document_create_element(dom_document *doc,
		dom_string *tag_name, dom_element **result)
{
	dom_html_document *html = (dom_html_document *) doc;

	if (_dom_validate_name(tag_name) == false)
		return DOM_INVALID_CHARACTER_ERR;

	return _dom_html_element_create(html, tag_name, NULL, NULL,
			(dom_html_element **) result);
}

dom_exception _dom_html_document_create_element_ns(dom_document *doc,
		dom_string *namespace, dom_string *qname,
		dom_element **result)
{
	dom_html_document *html = (dom_html_document *) doc;
	dom_string *prefix, *localname;
	dom_exception err;

	if (_dom_validate_name(qname) == false)
		return DOM_INVALID_CHARACTER_ERR;

	/* Validate qname */
	err = _dom_namespace_validate_qname(qname, namespace);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Divide QName into prefix/localname pair */
	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Attempt to create element */
	err = _dom_html_element_create(html, localname, namespace, prefix,
			(dom_html_element **) result);

	/* Tidy up */
	if (localname != NULL) {
		dom_string_unref(localname);
	}

	if (prefix != NULL) {
		dom_string_unref(prefix);
	}

	return err;
}

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
dom_exception _dom_html_document_get_title(dom_html_document *doc,
		dom_string **title)
{
	dom_exception exc = DOM_NO_ERR;
	*title = NULL;
	
	if (doc->title != NULL) {
		*title = dom_string_ref(doc->title);
	} else {
		dom_element *node;
		dom_string *title_str;
		dom_nodelist *nodes;
		unsigned long len;
		
		exc = dom_string_create_interned((uint8_t*)"title", 
						 5, &title_str);
		if (exc != DOM_NO_ERR) {
			return exc;
		}
		
		exc = dom_document_get_elements_by_tag_name(doc,
							    title_str,
							    &nodes);
		dom_string_unref(title_str);
		if (exc != DOM_NO_ERR) {
			return exc;
		}
		
		exc = dom_nodelist_get_length(nodes, &len);
		if (exc != DOM_NO_ERR) {
			dom_nodelist_unref(nodes);
			return exc;
		}
		
		if (len == 0) {
			dom_nodelist_unref(nodes);
			return DOM_NO_ERR;
		}
		
		exc = dom_nodelist_item(nodes, 0, &node);
		dom_nodelist_unref(nodes);
		if (exc != DOM_NO_ERR) {
			return exc;
		}
		
		exc = dom_node_get_text_content(node, title);
		dom_node_unref(node);
	}

	return exc;
}

dom_exception _dom_html_document_set_title(dom_html_document *doc,
		dom_string *title)
{
	if (doc->title != NULL)
		dom_string_unref(doc->title);

	doc->title = dom_string_ref(title);

	return DOM_NO_ERR;
}

dom_exception _dom_html_document_get_referrer(dom_html_document *doc,
		dom_string **referrer)
{
	*referrer = dom_string_ref(doc->referrer);

	return DOM_NO_ERR;
}

dom_exception _dom_html_document_get_domain(dom_html_document *doc,
		dom_string **domain)
{
	*domain = dom_string_ref(doc->domain);

	return DOM_NO_ERR;
}

dom_exception _dom_html_document_get_url(dom_html_document *doc,
		dom_string **url)
{
	*url = dom_string_ref(doc->url);

	return DOM_NO_ERR;
}

dom_exception _dom_html_document_get_body(dom_html_document *doc,
		struct dom_html_element **body)
{
	UNUSED(doc);
	UNUSED(body);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_set_body(dom_html_document *doc,
		struct dom_html_element *body)
{
	UNUSED(doc);
	UNUSED(body);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_get_images(dom_html_document *doc,
		struct dom_html_collection **col)
{
	UNUSED(doc);
	UNUSED(col);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_get_applets(dom_html_document *doc,
		struct dom_html_collection **col)
{
	UNUSED(doc);
	UNUSED(col);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_get_links(dom_html_document *doc,
		struct dom_html_collection **col)
{
	UNUSED(doc);
	UNUSED(col);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_get_forms(dom_html_document *doc,
		struct dom_html_collection **col)
{
	UNUSED(doc);
	UNUSED(col);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_get_anchors(dom_html_document *doc,
		struct dom_html_collection **col)
{
	UNUSED(doc);
	UNUSED(col);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_get_cookie(dom_html_document *doc,
		dom_string **cookie)
{
	UNUSED(doc);
	UNUSED(cookie);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_set_cookie(dom_html_document *doc,
		dom_string *cookie)
{
	UNUSED(doc);
	UNUSED(cookie);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_open(dom_html_document *doc)
{
	UNUSED(doc);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_close(dom_html_document *doc)
{
	UNUSED(doc);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_write(dom_html_document *doc,
		dom_string *text)
{
	UNUSED(doc);
	UNUSED(text);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_writeln(dom_html_document *doc,
		dom_string *text)
{
	UNUSED(doc);
	UNUSED(text);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception _dom_html_document_get_elements_by_name(dom_html_document *doc,
		dom_string *name, struct dom_nodelist **list)
{
	UNUSED(doc);
	UNUSED(name);
	UNUSED(list);

	return DOM_NOT_SUPPORTED_ERR;
}

