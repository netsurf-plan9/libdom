/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_utils_charset_errors_h_
#define dom_utils_charset_errors_h_

typedef enum {
	CHARSET_OK,		/**< No error */
	CHARSET_BADPARM,	/**< Bad parameters to argument */
	CHARSET_NEEDDATA,	/**< Insufficient data for operation */
	CHARSET_INVALID		/**< Invalid input data */
} charset_error;

#endif

