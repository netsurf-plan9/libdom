/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef xml_functypes_h_
#define xml_functypes_h_

/**
 * Type of XML parser allocation function
 */
typedef void *(*xml_alloc)(void *ptr, size_t len, void *pw);

#endif
