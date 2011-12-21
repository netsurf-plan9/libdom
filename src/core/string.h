
/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_internal_core_string_h_
#define dom_internal_core_string_h_

#include <dom/core/string.h>

/* Create a DOM string from a lwc_string */
dom_exception _dom_string_create_from_lwcstring(struct lwc_string_s *str, 
		dom_string **ret);

/* Map the lwc_error to dom_exception */
dom_exception _dom_exception_from_lwc_error(lwc_error err);

/**
 * Get the raw character data of the dom_string.
 * @note: This function is just provided for the convenience of accessing the 
 * raw C string character, no change on the result string is allowed.
 */
const char *_dom_string_data(dom_string *str);

/* Get the byte length of this dom_string */
size_t _dom_string_byte_length(dom_string *str);

#endif

