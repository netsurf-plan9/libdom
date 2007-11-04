/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_hubbub_errors_h_
#define dom_hubbub_errors_h_

typedef enum {
	DOM_HUBBUB_OK           = 0,

	DOM_HUBBUB_NOMEM        = 1,

	DOM_HUBBUB_HUBBUB_ERR   = (1<<16),
} dom_hubbub_error;

#endif
