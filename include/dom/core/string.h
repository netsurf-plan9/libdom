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
#include <libwapcaplet/libwapcaplet.h>

#include <dom/functypes.h>
#include <dom/core/exceptions.h>


typedef struct dom_string dom_string;

/* Claim a reference on a DOM string */
void dom_string_ref(dom_string *str);
/* Release a reference on a DOM string */
void dom_string_unref(dom_string *str);

/* Create a DOM string from a string of characters */
dom_exception dom_string_create(dom_alloc alloc, void *pw,
		const uint8_t *ptr, size_t len, dom_string **str);

/* Clone a dom_string */
dom_exception dom_string_clone(dom_alloc alloc, void *pw,
		dom_string *str, dom_string **ret);

/* Get the internal lwc_string */
dom_exception dom_string_get_intern(dom_string *str, 
		struct lwc_string_s **lwcstr);

/* Case sensitively compare two DOM strings */
int dom_string_cmp(dom_string *s1, dom_string *s2);
/* Case insensitively compare two DOM strings */
int dom_string_icmp(dom_string *s1, dom_string *s2);

/* Get the index of the first occurrence of a character in a dom string */
uint32_t dom_string_index(dom_string *str, uint32_t chr);
/* Get the index of the last occurrence of a character in a dom string */
uint32_t dom_string_rindex(dom_string *str, uint32_t chr);

/* Get the length, in characters, of a dom string */
uint32_t dom_string_length(dom_string *str);

/* Get the UCS-4 character at position index, the index should be in 
 * [0, length), and length can be get by calling dom_string_length
 */
dom_exception dom_string_at(dom_string *str, uint32_t index, 
		uint32_t *ch);

/* Concatenate two dom strings */
dom_exception dom_string_concat(dom_string *s1, dom_string *s2,
		dom_string **result);

/* Extract a substring from a dom string */
dom_exception dom_string_substr(dom_string *str, 
		uint32_t i1, uint32_t i2, dom_string **result);

/* Insert data into a dom string at the given location */
dom_exception dom_string_insert(dom_string *target,
		dom_string *source, uint32_t offset,
		dom_string **result);

/* Replace a section of a dom string */
dom_exception dom_string_replace(dom_string *target,
		dom_string *source, uint32_t i1, uint32_t i2,
		dom_string **result);

/* Duplicate a dom string */
dom_exception dom_string_dup(dom_string *str, 
		dom_string **result);

/* Calculate a hash value from a dom string */
uint32_t dom_string_hash(dom_string *str);

#endif
