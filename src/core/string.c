/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <ctype.h>
#include <inttypes.h>
#include <string.h>

#include <parserutils/charset/utf8.h>

#include <dom/core/string.h>

#include "core/document.h"
#include "utils/utils.h"

/**
 * A DOM string
 *
 * Strings are reference counted so destruction is performed correctly.
 */
struct dom_string {
	uint8_t *ptr;			/**< Pointer to string data */

	size_t len;			/**< Byte length of string */

	dom_alloc alloc;		/**< Memory (de)allocation function */
	void *pw;			/**< Client-specific data */

	uint32_t refcnt;		/**< Reference count */
};

static struct dom_string empty_string = { 
	.ptr = NULL,
	.len = 0,
	.alloc = NULL,
	.pw = NULL,
	.refcnt = 1
};

/**
 * Claim a reference on a DOM string
 *
 * \param str  The string to claim a reference on
 */
void dom_string_ref(struct dom_string *str)
{
	str->refcnt++;
}

/**
 * Release a reference on a DOM string
 *
 * \param str  The string to release the reference from
 *
 * If the reference count reaches zero, any memory claimed by the
 * string will be released
 */
void dom_string_unref(struct dom_string *str)
{
	if (--str->refcnt == 0) {
		if (str->alloc != NULL) {
			str->alloc(str->ptr, 0, str->pw);
			str->alloc(str, 0, str->pw);
		}
	}
}

/**
 * Create a DOM string from a string of characters
 *
 * \param alloc    Memory (de)allocation function
 * \param pw       Pointer to client-specific private data
 * \param ptr      Pointer to string of characters
 * \param len      Length, in bytes, of string of characters
 * \param str      Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will already be referenced, so there is no need
 * to explicitly reference it.
 *
 * The string of characters passed in will be copied for use by the 
 * returned DOM string.
 */
dom_exception dom_string_create(dom_alloc alloc, void *pw,
		const uint8_t *ptr, size_t len, struct dom_string **str)
{
	struct dom_string *ret;

	if (ptr == NULL && len == 0) {
		dom_string_ref(&empty_string);

		*str = &empty_string;

		return DOM_NO_ERR;
	}

	ret = alloc(NULL, sizeof(struct dom_string), pw);
	if (ret == NULL)
		return DOM_NO_MEM_ERR;

	ret->ptr = alloc(NULL, len, pw);
	if (ret->ptr == NULL) {
		alloc(ret, 0, pw);
		return DOM_NO_MEM_ERR;
	}

	memcpy(ret->ptr, ptr, len);

	ret->len = len;

	ret->alloc = alloc;
	ret->pw = pw;

	ret->refcnt = 1;

	*str = ret;

	return DOM_NO_ERR;
}

/**
 * Case sensitively compare two DOM strings
 *
 * \param s1  The first string to compare
 * \param s2  The second string to compare
 * \return 0 if strings match, non-0 otherwise
 *
 * NULL and "" will match.
 */
int dom_string_cmp(struct dom_string *s1, struct dom_string *s2)
{
	if (s1 == NULL)
		s1 = &empty_string;

	if (s2 == NULL)
		s2 = &empty_string;

	if (s1->len != s2->len)
		return 1;

	return memcmp(s1->ptr, s2->ptr, s1->len);
}

/**
 * Case insensitively compare two DOM strings
 *
 * \param s1  The first string to compare
 * \param s2  The second string to compare
 * \return 0 if strings match, non-0 otherwise
 *
 * NULL and "" will match.
 */
int dom_string_icmp(struct dom_string *s1, struct dom_string *s2)
{
	const uint8_t *d1 = NULL;
	const uint8_t *d2 = NULL;
	size_t l1, l2;

	if (s1 == NULL)
		s1 = &empty_string;
	if (s2 == NULL)
		s2 = &empty_string;

	d1 = s1->ptr;
	d2 = s2->ptr;
	l1 = s1->len;
	l2 = s2->len;

	while (l1 > 0 && l2 > 0) {
		uint32_t c1, c2;
		size_t cl1, cl2;
		parserutils_error err;

		err = parserutils_charset_utf8_to_ucs4(d1, l1, &c1, &cl1); 
		if (err != PARSERUTILS_OK) {
		}

		err = parserutils_charset_utf8_to_ucs4(d2, l2, &c2, &cl2);
		if (err != PARSERUTILS_OK) {
		}

		/** \todo improved lower-casing algorithm */
		if (tolower(c1) != tolower(c2)) {
			return (int)(tolower(c1) - tolower(c2));
		}

		d1 += cl1;
		d2 += cl2;

		l1 -= cl1;
		l2 -= cl2;
	}

	return (int)(l1 - l2);
}

/**
 * Get the index of the first occurrence of a character in a dom string 
 * 
 * \param str  The string to search in
 * \param chr  UCS4 value to look for
 * \return Character index of found character, or -1 if none found 
 */
uint32_t dom_string_index(struct dom_string *str, uint32_t chr)
{
	const uint8_t *s;
	size_t clen, slen;
	uint32_t c, index;
	parserutils_error err;

	if (str == NULL)
		str = &empty_string;

	s = str->ptr;
	slen = str->len;

	index = 0;

	while (slen > 0) {
		err = parserutils_charset_utf8_to_ucs4(s, slen, &c, &clen);
		if (err != PARSERUTILS_OK) {
			return (uint32_t) -1;
		}

		if (c == chr) {
			return index;
		}

		s += clen;
		slen -= clen;
		index++;
	}

	return (uint32_t) -1;
}

/**
 * Get the index of the last occurrence of a character in a dom string 
 * 
 * \param str  The string to search in
 * \param chr  UCS4 value to look for
 * \return Character index of found character, or -1 if none found
 */
uint32_t dom_string_rindex(struct dom_string *str, uint32_t chr)
{
	const uint8_t *s;
	size_t clen, slen;
	uint32_t c, index;
	parserutils_error err;

	if (str == NULL)
		str = &empty_string;

	s = str->ptr;
	slen = str->len;

	index = dom_string_length(str);

	while (slen > 0) {
		err = parserutils_charset_utf8_prev(s, slen, 
				(uint32_t *) &clen);
		if (err == PARSERUTILS_OK) {
			err = parserutils_charset_utf8_to_ucs4(s + clen, 
					slen - clen, &c, &clen);
		}

		if (err != PARSERUTILS_OK) {
			return (uint32_t) -1;
		}

		if (c == chr) {
			return index;
		}

		slen -= clen;
		index--;
	}

	return (uint32_t) -1;
}

/**
 * Get the length, in characters, of a dom string
 *
 * \param str  The string to measure the length of
 * \return The length of the string, in characters
 */
uint32_t dom_string_length(struct dom_string *str)
{
	size_t clen;
	parserutils_error err;

	if (str == NULL)
		str = &empty_string;

	err = parserutils_charset_utf8_length(str->ptr, str->len, &clen);
	if (err != PARSERUTILS_OK) {
		return 0;
	}

	return clen;
}

/** 
 * Concatenate two dom strings 
 * 
 * \param s1      The first string
 * \param s2      The second string
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will be allocated using the allocation details
 * stored in ::s1.
 * 
 * The returned string will have its reference count increased. The client
 * should dereference it once it has finished with it.
 */
dom_exception dom_string_concat(struct dom_string *s1, struct dom_string *s2,
		struct dom_string **result)
{
	struct dom_string *concat;

	concat = s1->alloc(NULL, sizeof(struct dom_string), s1->pw);

	if (concat == NULL) {
		return DOM_NO_MEM_ERR;
	}

	concat->ptr = s1->alloc(NULL, s1->len + s2->len, s1->pw);
	if (concat->ptr == NULL) {
		s1->alloc(concat, 0, s1->pw);

		return DOM_NO_MEM_ERR;
	}

	memcpy(concat->ptr, s1->ptr, s1->len);

	memcpy(concat->ptr + s1->len, s2->ptr, s2->len);

	concat->len = s1->len + s2->len;

	concat->alloc = s1->alloc;
	concat->pw = s1->pw;

	concat->refcnt = 1;

	*result = concat;

	return DOM_NO_ERR;
}

/**
 * Extract a substring from a dom string 
 *
 * \param str     The string to extract from
 * \param i1      The character index of the start of the substring
 * \param i2      The character index of the end of the substring
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will be allocated using the allocation details
 * stored in ::str.
 *
 * The returned string will have its reference count increased. The client
 * should dereference it once it has finished with it.
 */
dom_exception dom_string_substr(struct dom_string *str, 
		uint32_t i1, uint32_t i2, struct dom_string **result)
{
	const uint8_t *s = str->ptr;
	size_t slen = str->len;
	size_t b1, b2;
	parserutils_error err;

	/* Initialise the byte index of the start to 0 */
	b1 = 0;
	/* Make the end a character offset from the start */
	i2 -= i1;

	/* Calculate the byte index of the start */
	while (i1 > 0) {
		err = parserutils_charset_utf8_next(s, slen - b1, b1, 
				(uint32_t *) &b1);
		if (err != PARSERUTILS_OK) {
			return DOM_NO_MEM_ERR;
		}

		i1--;
	}

	/* Initialise the byte index of the end to that of the start */
	b2 = b1;

	/* Calculate the byte index of the end */
	while (i2 > 0) {
		err = parserutils_charset_utf8_next(s, slen - b2, b2, 
				(uint32_t *) &b2);

		if (err != PARSERUTILS_OK) {
			return DOM_NO_MEM_ERR;
		}

		i2--;
	}

	/* Create a string from the specified byte range */
	return dom_string_create(str->alloc, str->pw, s + b1, b2 - b1, result);
}

/**
 * Insert data into a dom string at the given location
 *
 * \param target  Pointer to string to insert into
 * \param source  Pointer to string to insert
 * \param offset  Character offset of location to insert at
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR          on success, 
 *         DOM_NO_MEM_ERR      on memory exhaustion,
 *         DOM_INDEX_SIZE_ERR  if ::offset > len(::target).
 *
 * The returned string will be allocated using the allocation details
 * stored in ::target.
 *
 * The returned string will have its reference count increased. The client
 * should dereference it once it has finished with it. 
 */
dom_exception dom_string_insert(struct dom_string *target,
		struct dom_string *source, uint32_t offset,
		struct dom_string **result)
{
	struct dom_string *res;
	const uint8_t *t, *s;
	uint32_t tlen, slen, clen;
	uint32_t ins = 0;
	parserutils_error err;

	t = target->ptr;
	tlen = target->len;
	s = source->ptr;
	slen = source->len;

	clen = dom_string_length(target);

	if (offset > clen)
		return DOM_INDEX_SIZE_ERR;

	/* Calculate the byte index of the insertion point */
	if (offset == clen) {
		/* Optimisation for append */
		offset = 0;
		ins = tlen;
	} else {
		while (offset > 0) {
			err = parserutils_charset_utf8_next(t, tlen - ins, 
					ins, &ins);

			if (err != PARSERUTILS_OK) {
				return DOM_NO_MEM_ERR;
			}

			offset--;
		}
	}

	/* Allocate result string */
	res = target->alloc(NULL, sizeof(struct dom_string), target->pw);
	if (res == NULL) {
		return DOM_NO_MEM_ERR;
	}

	/* Allocate data buffer for result contents */
	res->ptr = target->alloc(NULL, tlen + slen, target->pw);
	if (res->ptr == NULL) {
		target->alloc(res, 0, target->pw);
		return DOM_NO_MEM_ERR;
	}

	/* Copy initial portion of target, if any, into result */
	if (ins > 0) {
		memcpy(res->ptr, t, ins);
	}

	/* Copy inserted data into result */
	memcpy(res->ptr + ins, s, slen);

	/* Copy remainder of target, if any, into result */
	if (tlen - ins > 0) {
		memcpy(res->ptr + ins + slen, t + ins, tlen - ins);
	}

	res->len = tlen + slen;

	res->alloc = target->alloc;
	res->pw = target->pw;
	
	res->refcnt = 1;

	*result = res;

	return DOM_NO_ERR;
}

/** 
 * Replace a section of a dom string
 *
 * \param target  Pointer to string of which to replace a section
 * \param source  Pointer to replacement string
 * \param i1      Character index of start of region to replace
 * \param i2      Character index of end of region to replace
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion.
 *
 * The returned string will be allocated using the allocation details
 * stored in ::target.
 *
 * The returned string will have its reference count increased. The client
 * should dereference it once it has finished with it. 
 */
dom_exception dom_string_replace(struct dom_string *target,
		struct dom_string *source, uint32_t i1, uint32_t i2,
		struct dom_string **result)
{
	struct dom_string *res;
	const uint8_t *t, *s;
	uint32_t tlen, slen;
	uint32_t b1, b2;
	parserutils_error err;

	t = target->ptr;
	tlen = target->len;
	s = source->ptr;
	slen = source->len;

	/* Initialise the byte index of the start to 0 */
	b1 = 0;
	/* Make the end a character offset from the start */
	i2 -= i1;

	/* Calculate the byte index of the start */
	while (i1 > 0) {
		err = parserutils_charset_utf8_next(s, slen - b1, b1, &b1);

		if (err != PARSERUTILS_OK) {
			return DOM_NO_MEM_ERR;
		}

		i1--;
	}

	/* Initialise the byte index of the end to that of the start */
	b2 = b1;

	/* Calculate the byte index of the end */
	while (i2 > 0) {
		err = parserutils_charset_utf8_next(s, slen - b2, b2, &b2);

		if (err != PARSERUTILS_OK) {
			return DOM_NO_MEM_ERR;
		}

		i2--;
	}

	/* Allocate result string */
	res = target->alloc(NULL, sizeof(struct dom_string), target->pw);

	if (res == NULL) {
		return DOM_NO_MEM_ERR;
	}

	/* Allocate data buffer for result contents */
	res->ptr = target->alloc(NULL, tlen + slen - (b2 - b1), target->pw);
	if (res->ptr == NULL) {
		target->alloc(res, 0, target->pw);
		return DOM_NO_MEM_ERR;
	}

	/* Copy initial portion of target, if any, into result */
	if (b1 > 0) {
		memcpy(res->ptr, t, b1);
	}

	/* Copy replacement data into result */
	if (slen > 0) {
		memcpy(res->ptr + b1, s, slen);
	}

	/* Copy remainder of target, if any, into result */
	if (tlen - b2 > 0) {
		memcpy(res->ptr + b1 + slen, t + b2, tlen - b2);
	}

	res->len = tlen + slen - (b2 - b1);

	res->alloc = target->alloc;
	res->pw = target->pw;

	res->refcnt = 1;

	*result = res;

	return DOM_NO_ERR;
}

/**
 * Duplicate a dom string 
 *
 * \param str     The string to duplicate
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will be allocated using the allocation details
 * stored in ::str.
 *
 * The returned string will have its reference count increased. The client
 * should dereference it once it has finished with it.
 */
dom_exception dom_string_dup(struct dom_string *str, 
		struct dom_string **result)
{
	return dom_string_create(str->alloc, str->pw, str->ptr, str->len, 
			result);
}

/**
 * Calculate a hash value from a dom string 
 *
 * \param str  The string to calculate a hash of
 * \return The hash value associated with the string
 */
uint32_t dom_string_hash(struct dom_string *str)
{
	const uint8_t *s = str->ptr;
	size_t slen = str->len;
	uint32_t hash = 0x01000193;

	while (slen > 0) {
		hash *= 0x01000193;
		hash ^= *s;

		s++;
		slen--;
	}

	return hash;
}

