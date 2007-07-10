/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_string_h_
#define dom_string_h_

#include <inttypes.h>
#include <stddef.h>

#include <dom/core/exceptions.h>

struct dom_document;
struct dom_string;

/* Claim a reference on a DOM string */
void dom_string_ref(struct dom_string *str);
/* Release a reference on a DOM string */
void dom_string_unref(struct dom_string *str);

/* Create a DOM string from an offset into the document buffer */
dom_exception dom_string_create_from_off(struct dom_document *doc,
		uint32_t off, size_t len, struct dom_string **str);
/* Create a DOM string from a string of characters */
dom_exception dom_string_create_from_ptr(struct dom_document *doc,
		const uint8_t *ptr, size_t len, struct dom_string **str);
/* Create a DOM string from a constant string of characters */
dom_exception dom_string_create_from_const_ptr(struct dom_document *doc,
		const uint8_t *ptr, size_t len, struct dom_string **str);

/* Get a pointer to the string of characters within a DOM string */
dom_exception dom_string_get_data(struct dom_string *str,
		const uint8_t **data, size_t *len);

/* Case sensitively compare two DOM strings */
int dom_string_cmp(struct dom_string *s1, struct dom_string *s2);
/* Case insensitively compare two DOM strings */
int dom_string_icmp(struct dom_string *s1, struct dom_string *s2);


#endif
