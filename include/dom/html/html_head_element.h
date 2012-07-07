/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku.com>
 */

#ifndef dom_html_head_element_h_
#define dom_html_head_element_h_

#include <dom/html/html_element.h>

typedef struct dom_html_head_element dom_html_head_element;

typedef struct dom_html_head_element_vtable {
        struct dom_html_element_vtable base;

        dom_exception (*dom_html_head_element_get_profile)(struct dom_html_head_element *element,
                                                 dom_string **profile);
        dom_exception (*dom_html_head_element_set_profile)(struct dom_html_head_element *element,
                                                 dom_string *profile);
} dom_html_head_element_vtable;

static inline dom_exception dom_html_head_element_get_profile(
		struct dom_html_head_element *element, dom_string **profile)
{
        return ((dom_html_head_element_vtable *) ((dom_node *) element)->vtable)->
                dom_html_head_element_get_profile(element, profile);
}
#define dom_html_head_element_get_profile(e, profile) \
	dom_html_head_element_get_profile((dom_html_head_element *) (e), (profile))

static inline dom_exception dom_html_head_element_set_profile(
		struct dom_html_head_element *element, dom_string *profile)
{
        return ((dom_html_head_element_vtable *) ((dom_node *) element)->vtable)->
                dom_html_head_element_set_profile(element, profile);
}
#define dom_html_head_element_set_profile(e, profile) \
	dom_html_head_element_set_profile((dom_html_head_element *) (e), (profile))


#endif

