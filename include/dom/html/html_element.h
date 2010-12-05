/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_html_element_h_
#define dom_html_element_h_

#include <dom/core/element.h>

typedef struct dom_html_element dom_html_element;

typedef struct dom_html_element_vtable {
        struct dom_element_vtable base;
        
        dom_exception (*dom_html_element_get_id)(struct dom_html_element *element,
                                                 struct dom_string **id);
        dom_exception (*dom_html_element_set_id)(struct dom_html_element *element,
                                                 struct dom_string *id);
};

static inline dom_exception dom_html_element_get_id(struct dom_html_element *element,
                                                    struct dom_string **id)
{
        return ((dom_html_element_vtable *) ((dom_node *) element)->vtable)->
                dom_html_element_get_id(element, id);
}
#define dom_html_element_get_id(e, id) dom_html_element_get_id( \
		(dom_html_element *) (e), (struct dom_string **) (id))

static inline dom_exception dom_html_element_set_id(struct dom_html_element *element,
                                                    struct dom_string *id)
{
        return ((dom_html_element_vtable *) ((dom_node *) element)->vtable)->
                dom_html_element_set_id(element, id);
}
#define dom_html_element_set_id(e, id) dom_html_element_set_id( \
		(dom_html_element *) (e), (struct dom_string *) (id))

#endif

