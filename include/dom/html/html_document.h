/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_html_document_h_
#define dom_html_document_h_

#include <dom/core/document.h>
#include <dom/core/exceptions.h>
#include <dom/functypes.h>
#include <dom/events/document_event.h>

struct dom_element;
struct dom_html_collection;
struct dom_html_element;
struct dom_nodelist;

typedef struct dom_html_document dom_html_document;

typedef struct dom_html_document_vtable {
	struct dom_document_vtable base;

	dom_exception (*get_title)(dom_html_document *doc,
			dom_string **title);
	dom_exception (*set_title)(dom_html_document *doc,
			dom_string *title);
	dom_exception (*get_referer)(dom_html_document *doc,
			dom_string **referer);
	dom_exception (*get_domain)(dom_html_document *doc,
			dom_string **domain);
	dom_exception (*get_url)(dom_html_document *doc,
			dom_string **url);
	dom_exception (*get_body)(dom_html_document *doc,
			struct dom_html_element **body);
	dom_exception (*set_body)(dom_html_document *doc,
			struct dom_html_element *body);
	dom_exception (*get_images)(dom_html_document *doc,
			struct dom_html_collection **col);
	dom_exception (*get_applets)(dom_html_document *doc,
			struct dom_html_collection **col);
	dom_exception (*get_links)(dom_html_document *doc,
			struct dom_html_collection **col);
	dom_exception (*get_forms)(dom_html_document *doc,
			struct dom_html_collection **col);
	dom_exception (*get_anchors)(dom_html_document *doc,
			struct dom_html_collection **col);
	dom_exception (*get_cookie)(dom_html_document *doc,
			dom_string **cookie);
	dom_exception (*set_cookie)(dom_html_document *doc,
			dom_string *cookie);

	dom_exception (*open)(dom_html_document *doc);
	dom_exception (*close)(dom_html_document *doc);
	dom_exception (*write)(dom_html_document *doc,
			dom_string *text);
	dom_exception (*writeln)(dom_html_document *doc,
			dom_string *text);
	dom_exception (*get_elements_by_name)(dom_html_document *doc,
			dom_string *name, struct dom_nodelist **list);
} dom_html_document_vtable;

static inline dom_exception dom_html_document_get_title(
		dom_html_document *doc, dom_string **title)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_title(doc, title);
}
#define dom_html_document_get_title(d, t) \
		dom_html_document_get_title((dom_html_document *) (d), \
				(dom_string **) (t))

static inline dom_exception dom_html_document_set_title(dom_html_document *doc,
		dom_string *title)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		set_title(doc, title);
}
#define dom_html_document_set_title(d, t) \
		dom_html_document_set_title((dom_html_document *) (d), \
				(dom_string **) (t))

static inline dom_exception dom_html_document_get_referer(dom_html_document *doc,
		dom_string **referer)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_referer(doc, referer);
}
#define dom_html_document_get_referer(d, r) \
		dom_html_document_get_referer((dom_html_document *) (d), \
				(dom_string **) (r))

static inline dom_exception dom_html_document_get_domain(dom_html_document *doc,
		dom_string **domain)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_domain(doc, domain);
}
#define dom_html_document_get_domain(d, t) \
		dom_html_document_get_domain((dom_html_document *) (d), \
				(dom_string **) (t))

static inline dom_exception dom_html_document_get_url(dom_html_document *doc,
		dom_string **url)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_url(doc, url);
}
#define dom_html_document_get_url(d, u) \
		dom_html_document_get_url((dom_html_document *) (d), \
				(dom_string **) (u))

static inline dom_exception dom_html_document_get_body(dom_html_document *doc,
		struct dom_html_element **body)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_body(doc, body);
}
#define dom_html_document_get_body(d, b) \
		dom_html_document_get_title((dom_html_document *) (d), \
				(struct dom_html_element **) (b))

static inline dom_exception dom_html_document_set_body(dom_html_document *doc,
		struct dom_html_element *body)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		set_body(doc, body);
}
#define dom_html_document_set_body(d, b) \
		dom_html_document_set_body((dom_html_document *) (d), \
				(struct dom_html_element **) (b))

static inline dom_exception dom_html_document_get_images(dom_html_document *doc,
		struct dom_html_collection **col)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_images(doc, col);
}
#define dom_html_document_get_images(d, c) \
		dom_html_document_get_images((dom_html_document *) (d), \
				(struct dom_html_collection **) (c))

static inline dom_exception dom_html_document_get_applets(dom_html_document *doc,
		struct dom_html_collection **col)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_applets(doc, col);
}
#define dom_html_document_get_applets(d, c) \
		dom_html_document_get_applets((dom_html_document *) (d), \
				(struct dom_html_collection **) (c))

static inline dom_exception dom_html_document_get_links(dom_html_document *doc,
		struct dom_html_collection **col)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_links(doc, col);
}
#define dom_html_document_get_links(d, c) \
		dom_html_document_get_links((dom_html_document *) (d), \
				(struct dom_html_collection **) (c))

static inline dom_exception dom_html_document_get_forms(dom_html_document *doc,
		struct dom_html_collection **col)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_forms(doc, col);
}
#define dom_html_document_get_forms(d, c) \
		dom_html_document_get_forms((dom_html_document *) (d), \
				(struct dom_html_collection **) (c))

static inline dom_exception dom_html_document_get_anchors(dom_html_document *doc,
		struct dom_html_collection **col)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_anchors(doc, col);
}
#define dom_html_document_get_anchors(d, c) \
		dom_html_document_get_title((dom_html_document *) (d), \
				(struct dom_html_collection **) (c))

static inline dom_exception dom_html_document_get_cookie(dom_html_document *doc,
		dom_string **cookie)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_cookie(doc, cookie);
}
#define dom_html_document_get_cookie(d, c) \
		dom_html_document_get_title((dom_html_document *) (d), \
				(dom_string **) (c))

static inline dom_exception dom_html_document_set_cookie(dom_html_document *doc,
		dom_string *cookie)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		set_cookie(doc, cookie);
}
#define dom_html_document_set_cookie(d, c) \
		dom_html_document_set_cookie((dom_html_document *) (d), \
				(dom_string **) (c))


static inline dom_exception dom_html_document_open(dom_html_document *doc)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		open(doc);
}
#define dom_html_document_open(d) \
		dom_html_document_open((dom_html_document *) (d))

static inline dom_exception dom_html_document_close(dom_html_document *doc)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		close(doc);
}
#define dom_html_document_close(d) \
		dom_html_document_close((dom_html_document *) (d))


static inline dom_exception dom_html_document_write(dom_html_document *doc,
		dom_string *text)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		write(doc, text);
}
#define dom_html_document_write(d, t) \
		dom_html_document_write((dom_html_document *) (d), \
			(dom_string *) (t))

static inline dom_exception dom_html_document_writeln(dom_html_document *doc,
		dom_string *text)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		writeln(doc, text);
}
#define dom_html_document_writeln(d, t) \
		dom_html_document_writeln((dom_html_document *) (d), \
			(dom_string *) (t))

static inline dom_exception dom_html_document_get_elements_by_name(dom_html_document *doc,
		dom_string *name, struct dom_nodelist **list)
{
	return ((dom_html_document_vtable *) ((dom_node *) doc)->vtable)->
		get_elements_by_name(doc, name, list);
}
#define dom_html_document_get_elements_by_name(d, n, l) \
		dom_html_document_get_element_by_name((dom_html_document *) (d), \
			(dom_string *) (n), (struct dom_nodelist **) (l))

#endif

