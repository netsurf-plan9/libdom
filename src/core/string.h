/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_internal_core_string_h_
#define dom_internal_core_string_h_

#include <dom/core/string.h>

/* Create a DOM string from a lwc_string
 * This function call mainly used for create a string from lwc_string */
dom_exception _dom_string_create_from_lwcstring(dom_alloc alloc, void *pw,
		struct lwc_context_s *ctx, struct lwc_string_s *str, 
		struct dom_string **ret);

/* Make the dom_string be interned in the lwc_context */
dom_exception _dom_string_intern(struct dom_string *str, 
		struct lwc_context_s *ctx, struct lwc_string_s **lwcstr);

/* Compare the raw data of two lwc_strings for equality when the two strings
 * belong to different lwc_context */
int _dom_lwc_string_compare_raw(struct lwc_string_s *s1,
		struct lwc_string_s *s2);

/* Map the lwc_error to dom_exception */
dom_exception _dom_exception_from_lwc_error(lwc_error err);

#endif

