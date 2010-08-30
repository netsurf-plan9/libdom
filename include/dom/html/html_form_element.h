/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku.com>
 */

#ifndef dom_html_form_element_h_
#define dom_html_form_element_h_

#include <dom/core/exceptions.h>

struct dom_html_collection;

typedef struct dom_html_form_element dom_html_form_element;

dom_exception dom_html_form_element_get_elements(dom_html_form_element *ele,
		struct dom_html_collection **col);
dom_exception dom_html_form_element_get_length(dom_html_form_element *ele,
		unsigned long *len);
dom_exception dom_html_form_element_submit(dom_html_form_element *ele);
dom_exception dom_html_form_element_reset(dom_html_form_element *ele);

#endif

