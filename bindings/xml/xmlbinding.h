/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef xml_xmlbinding_h_
#define xml_xmlbinding_h_

#include "xmlerror.h"

/* Initialise the XML DOM binding */
dom_xml_error dom_xml_binding_initialise(dom_alloc alloc, void *pw);

/* Finalise the XML DOM binding */
dom_xml_error dom_xml_binding_finalise(void);

#endif
