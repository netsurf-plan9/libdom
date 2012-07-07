/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_internal_html_element_h_
#define dom_internal_html_element_h_

#include <dom/html/html_element.h>

#include "core/element.h"

struct dom_html_document;

/**
 * The dom_html_element class
 *
 */
struct dom_html_element {
	struct dom_element base;
			/**< The base class */
};

dom_exception _dom_html_element_create(struct dom_html_document *doc,
		dom_string *name, dom_string *namespace,
		dom_string *prefix, dom_html_element **result);

dom_exception _dom_html_element_initialise(struct dom_html_document *doc,
		struct dom_html_element *el, dom_string *name, 
		dom_string *namespace, dom_string *prefix);

void _dom_html_element_finalise(struct dom_html_element *ele);

/* The protected virtual functions */
void _dom_html_element_destroy(dom_node_internal *node);
dom_exception _dom_html_element_copy(dom_node_internal *old,
		dom_node_internal **copy);

#define DOM_HTML_ELEMENT_PROTECT_VTABLE \
	_dom_html_element_destroy, \
	_dom_html_element_copy


/* The API functions */
dom_exception _dom_html_element_get_id(dom_html_element *element,
                                       dom_string **id);
dom_exception _dom_html_element_set_id(dom_html_element *element,
                                       dom_string *id);
dom_exception _dom_html_element_get_title(dom_html_element *element,
                                       dom_string **title);
dom_exception _dom_html_element_set_title(dom_html_element *element,
                                       dom_string *title);
dom_exception _dom_html_element_get_lang(dom_html_element *element,
                                       dom_string **lang);
dom_exception _dom_html_element_set_lang(dom_html_element *element,
                                       dom_string *lang);
dom_exception _dom_html_element_get_dir(dom_html_element *element,
                                       dom_string **dir);
dom_exception _dom_html_element_set_dir(dom_html_element *element,
                                       dom_string *dir);
dom_exception _dom_html_element_get_class_name(dom_html_element *element,
                                       dom_string **class_name);
dom_exception _dom_html_element_set_class_name(dom_html_element *element,
                                       dom_string *class_name);

#define DOM_HTML_ELEMENT_VTABLE \
	_dom_html_element_get_id, \
	_dom_html_element_set_id, \
	_dom_html_element_get_title, \
	_dom_html_element_set_title, \
	_dom_html_element_get_lang, \
	_dom_html_element_set_lang, \
	_dom_html_element_get_dir, \
	_dom_html_element_set_dir, \
	_dom_html_element_get_class_name, \
	_dom_html_element_set_class_name

/* Some common functions used by all child classes */
dom_exception dom_html_element_get_bool_property(dom_html_element *ele,
		const char *name, unsigned long len, bool *has);
dom_exception dom_html_element_set_bool_property(dom_html_element *ele,
		const char *name, unsigned long len, bool has);

dom_exception dom_html_element_get_long_property(dom_html_element *ele,
		const char *name, unsigned long len, unsigned long *value);
dom_exception dom_html_element_set_long_property(dom_html_element *ele,
		const char *name, unsigned long len, unsigned long value);

extern struct dom_html_element_vtable _dom_html_element_vtable;

#endif

