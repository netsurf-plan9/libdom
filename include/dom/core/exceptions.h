/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_exceptions_h_
#define dom_core_exceptions_h_

/* The DOM spec says that this is actually an unsigned short */
typedef enum {
	DOM_NO_ERR			=  0,
	DOM_INDEX_SIZE_ERR		=  1,
	DOM_DOMSTRING_SIZE_ERR		=  2,
	DOM_HIERARCHY_REQUEST_ERR	=  3,
	DOM_WRONG_DOCUMENT_ERR		=  4,
	DOM_INVALID_CHARACTER_ERR	=  5,
	DOM_NO_DATA_ALLOWED_ERR		=  6,
	DOM_NO_MODIFICATION_ALLOWED_ERR	=  7,
	DOM_NOT_FOUND_ERR		=  8,
	DOM_NOT_SUPPORTED_ERR		=  9,
	DOM_INUSE_ATTRIBUTE_ERR		= 10,
	DOM_INVALID_STATE_ERR		= 11,
	DOM_SYNTAX_ERR			= 12,
	DOM_INVALID_MODIFICATION_ERR	= 13,
	DOM_NAMESPACE_ERR		= 14,
	DOM_INVALID_ACCESS_ERR		= 15,
	DOM_VALIDATION_ERR		= 16,
	DOM_TYPE_MISMATCH_ERR		= 17,
	DOM_NO_MEM_ERR			= (1<<16)
			/* our own internal error */
} dom_exception;

#endif
