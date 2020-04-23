/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2020 Vincent Sanders <vince@netsurf-browser.org>
 */

#ifndef dom_html_canvas_element_h_
#define dom_html_canvas_element_h_

#include <stdbool.h>
#include <dom/core/exceptions.h>

#include <dom/html/html_document.h>

typedef struct dom_html_canvas_element dom_html_canvas_element;

dom_exception dom_html_canvas_element_get_width(
	dom_html_canvas_element *object, int32_t *width);

dom_exception dom_html_canvas_element_set_width(
	dom_html_canvas_element *object, uint32_t width);

dom_exception dom_html_canvas_element_get_height(
	dom_html_canvas_element *object, int32_t *height);

dom_exception dom_html_canvas_element_set_height(
	dom_html_canvas_element *object, uint32_t height);


#endif
