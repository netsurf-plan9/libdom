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

#include <dom/functypes.h>
#include <dom/core/exceptions.h>

struct dom_document;
struct dom_string;

typedef enum {
	DOM_STRING_UTF8,
	DOM_STRING_UTF16
} dom_string_charset;

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
/* Create a DOM string from a string of characters that does not belong
 * to a document */
dom_exception dom_string_create_from_ptr_no_doc(dom_alloc alloc, void *pw,
		dom_string_charset charset, const uint8_t *ptr, size_t len, 
		struct dom_string **str);

/* Case sensitively compare two DOM strings */
int dom_string_cmp(struct dom_string *s1, struct dom_string *s2);
/* Case insensitively compare two DOM strings */
int dom_string_icmp(struct dom_string *s1, struct dom_string *s2);

/* Get the index of the first occurrence of a character in a dom string */
uint32_t dom_string_index(struct dom_string *str, uint32_t chr);
/* Get the index of the last occurrence of a character in a dom string */
uint32_t dom_string_rindex(struct dom_string *str, uint32_t chr);

/* Get the length, in characters, of a dom string */
uint32_t dom_string_length(struct dom_string *str);

/* Concatenate two dom strings */
dom_exception dom_string_concat(struct dom_string *s1, struct dom_string *s2,
		struct dom_string **result);

/* Extract a substring from a dom string */
dom_exception dom_string_substr(struct dom_string *str, 
		uint32_t i1, uint32_t i2, struct dom_string **result);

/* Insert data into a dom string at the given location */
dom_exception dom_string_insert(struct dom_string *target,
		struct dom_string *source, uint32_t offset,
		struct dom_string **result);

/* Replace a section of a dom string */
dom_exception dom_string_replace(struct dom_string *target,
		struct dom_string *source, uint32_t i1, uint32_t i2,
		struct dom_string **result);

/* Duplicate a dom string */
dom_exception dom_string_dup(struct dom_string *str, 
		struct dom_string **result);

/* Calculate a hash value from a dom string */
uint32_t dom_string_hash(struct dom_string *str);

#endif
