/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_utils_namespace_h_
#define dom_utils_namespace_h_

#include <dom/functypes.h>
#include <dom/core/exceptions.h>

struct dom_document;
struct dom_string;

/* Initialise the namespace component */
dom_exception _dom_namespace_initialise(dom_alloc alloc, void *pw);

/* Finalise the namespace component */
dom_exception _dom_namespace_finalise(void);

/* Ensure a QName is valid */
dom_exception _dom_namespace_validate_qname(struct dom_string *qname,
		struct dom_string *namespace);

/* Split a QName into a namespace prefix and localname string */
dom_exception _dom_namespace_split_qname(struct dom_string *qname,
		struct dom_document *doc, struct dom_string **prefix, 
		struct dom_string **localname);

#endif

