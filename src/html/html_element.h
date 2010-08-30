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

struct dom_html_form_element;

/**
 * The dom_html_element class
 *
 * Note: For now, there is no new members and methods in this class,
 * but in future, we may add common abstracted functions from child class
 * to here.
 */
struct dom_html_element {
	struct dom_element base;
			/**< The base class */
	struct dom_html_form_element *form;
			/**< The form element which contains this element if
			 *   this element is a form control
			 */
};

dom_exception _dom_html_element_initialise(struct dom_document *doc,
		struct dom_html_element *el, struct lwc_string_s *name, 
		struct lwc_string_s *namespace, struct lwc_string_s *prefix);

void _dom_html_element_finalise(struct dom_document *doc,
		struct dom_html_element *ele);

/* The protected virtual functions */
void _dom_virtual_html_element_destroy(dom_node_internal *node);
dom_exception _dom_html_element_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret);
dom_exception _dom_html_element_copy(struct dom_node_internal *new,
		struct dom_node_internal *old);

/* Some common functions used by all child classes */
dom_exception dom_html_element_get_bool_property(dom_html_element *ele,
		const char *name, unsigned long len, bool *has);
dom_exception dom_html_element_set_bool_property(dom_html_element *ele,
		const char *name, unsigned long len, bool has);
void _dom_html_element_get_form(dom_html_element *ele,
		struct dom_html_form_element **form);
void _dom_html_element_associate_form(dom_html_element *ele,
		struct dom_html_form_element *form);

#endif

