/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_html_element_h_
#define dom_html_element_h_

typedef struct dom_html_element dom_html_element;

/**
 * Note: DOM HTML spec is used to provide a more convenient way to
 * access Element's attribute through property. But, for implementation like
 * C, such propery provide no more convenience than Element.get(set)Attribute
 * function, so we ignore all the propety whose type is DOMString in this 
 * implementation, clients can always access these property through 
 * get(set)Attribute methods. 
 *
 * For the readonly property, an readonly Attr node should be created, so once
 * these Attr nodes are created, they can not be changed any more.
 */

#endif

