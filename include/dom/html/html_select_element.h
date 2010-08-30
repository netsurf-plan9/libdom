/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_html_select_element_h_
#define dom_html_select_element_h_

#include <stdbool.h>

#include <dom/core/exceptions.h>

typedef struct dom_html_select_element dom_html_select_element;

struct dom_html_options_collection;
struct dom_html_element;

dom_exception dom_html_select_element_get_selected_index(
		dom_html_select_element *ele, unsigned long *index);
dom_exception dom_html_select_element_set_selected_index(
		dom_html_select_element *ele, unsigned long index);
dom_exception dom_html_select_element_get_length(
		dom_html_select_element *ele, unsigned long *len);
dom_exception dom_html_select_element_set_length(
		dom_html_select_element *ele, unsigned long len);
dom_exception dom_html_select_element_get_options(
		dom_html_select_element *ele,
		struct dom_html_options_collection **col);
dom_exception dom_html_select_element_get_disabled(
		dom_html_select_element *ele, bool *disabled);
dom_exception dom_html_select_element_set_disabled(
		dom_html_select_element *ele, bool disabled);
dom_exception dom_html_select_element_get_multiple(
		dom_html_select_element *ele, bool *multiple);
dom_exception dom_html_select_element_set_multiple(
		dom_html_select_element *ele, bool multiple);
dom_exception dom_html_select_element_get_size(
		dom_html_select_element *ele, unsigned long *size);
dom_exception dom_html_select_element_set_size(
		dom_html_select_element *ele, unsigned long size);
dom_exception dom_html_select_element_get_tab_index(
		dom_html_select_element *ele, unsigned long *tab_index);
dom_exception dom_html_select_element_set_tab_index(
		dom_html_select_element *ele, unsigned long tab_index);

/* Functions */
dom_exception dom_html_select_element_add(struct dom_html_element *ele,
		struct dom_html_element *before);
dom_exception dom_html_element_blur(struct dom_html_select_element *ele);
dom_exception dom_html_element_focus(struct dom_html_select_element *ele);

#endif

