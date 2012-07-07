/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku.com>
 */

#ifndef dom_html_html_element_h_
#define dom_html_html_element_h_

#include <dom/html/html_element.h>

typedef struct dom_html_html_element dom_html_html_element;

typedef struct dom_html_html_element_vtable {
        struct dom_html_element_vtable base;

        dom_exception (*dom_html_html_element_get_version)(struct dom_html_html_element *element,
                                                 dom_string **version);
        dom_exception (*dom_html_html_element_set_version)(struct dom_html_html_element *element,
                                                 dom_string *version);
} dom_html_html_element_vtable;

static inline dom_exception dom_html_html_element_get_version(
		struct dom_html_html_element *element, dom_string **version)
{
        return ((dom_html_html_element_vtable *) ((dom_node *) element)->vtable)->
                dom_html_html_element_get_version(element, version);
}
#define dom_html_html_element_get_version(e, version) \
	dom_html_html_element_get_version((dom_html_html_element *) (e), (version))

static inline dom_exception dom_html_html_element_set_version(
		struct dom_html_html_element *element, dom_string *version)
{
        return ((dom_html_html_element_vtable *) ((dom_node *) element)->vtable)->
                dom_html_html_element_set_version(element, version);
}
#define dom_html_html_element_set_version(e, version) \
	dom_html_html_element_set_version((dom_html_html_element *) (e), (version))


#endif

