/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef xml_xmlbinding_h_
#define xml_xmlbinding_h_

#include "xmlerror.h"
#include "functypes.h"

/* Initialise the XML DOM binding */
xml_error xml_dom_binding_initialise(xml_alloc alloc, void *pw);

/* Finalise the XML DOM binding */
xml_error xml_dom_binding_finalise(void);

#endif
