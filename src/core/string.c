/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <string.h>

#include <parserutils/charset/utf8.h>

#include "core/string.h"
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

	lwc_string *intern;		/**< The lwc_string of this string */

	lwc_context *context;	/**< The lwc_context for the lwc_string */

	dom_alloc alloc;		/**< Memory (de)allocation function */
	void *pw;			/**< Client-specific data */

	uint32_t refcnt;		/**< Reference count */
};

static struct dom_string empty_string = { 
	.ptr = NULL,
	.len = 0,
	.intern = NULL,
	.context = NULL,
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
	if (str == NULL)
		return;
	
	if (--str->refcnt == 0) {
		if (str->intern != NULL) {
			lwc_context_unref(str->context);
			lwc_context_string_unref(str->context, str->intern);
			str->alloc(str, 0, str->pw);
		} else if (str->alloc != NULL) {
			str->alloc(str->ptr, 0, str->pw);
			str->alloc(str, 0, str->pw);
		}
	}
}

/**
 * Create a DOM string from a string of characters
 *
 * \param alloc  Memory (de)allocation function
 * \param pw     Pointer to client-specific private data
 * \param ptr    Pointer to string of characters
 * \param len    Length, in bytes, of string of characters
 * \param str    Pointer to location to receive result
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

	if (ptr == NULL || len == 0) {
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

	ret->intern = NULL;
	ret->context = NULL;

	ret->refcnt = 1;

	*str = ret;

	return DOM_NO_ERR;
}

/**
 * Clone a dom_string if necessary. This method is used to create a new string
 * with a new allocator, but if the allocator is the same with the paramter 
 * str, just ref the string.
 *
 * \param alloc  The new allocator for this string
 * \param pw     The new pw for this string
 * \param str    The source dom_string
 * \param ret    The cloned dom_string
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * @note: When both the alloc and pw are the same as the str's, we need no
 *	  real clone, just ref the source string is ok.
 */
dom_exception dom_string_clone(dom_alloc alloc, void *pw, 
		struct dom_string *str, struct dom_string **ret)
{
	if (alloc == str->alloc && pw == str->pw) {
		*ret = str;
		dom_string_ref(str);
		return DOM_NO_ERR;
	}

	if (str->intern != NULL) {
		return _dom_string_create_from_lwcstring(alloc, pw,
				str->context, str->intern, ret);
	} else {
		return dom_string_create(alloc, pw, str->ptr, str->len, ret);
	}
}

/**
 * Create a dom_string from a lwc_string
 * 
 * \param ctx  The lwc_context
 * \param str  The lwc_string
 * \param ret  The new dom_string
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 */
dom_exception _dom_string_create_from_lwcstring(dom_alloc alloc, void *pw,
		lwc_context *ctx, lwc_string *str, struct dom_string **ret)
{
	dom_string *r;

	if (str == NULL) {
		*ret = NULL;
		return DOM_NO_ERR;
	}

	r = alloc(NULL, sizeof(struct dom_string), pw);
	if (r == NULL)
		return DOM_NO_MEM_ERR;

	if (str == NULL) {
		*ret = &empty_string;
		dom_string_ref(*ret);
		return DOM_NO_ERR;
	}

	r->context = ctx;
	r->intern = str;
	r->ptr = (uint8_t *)lwc_string_data(str);
	r->len = lwc_string_length(str);

	r->alloc = alloc;
	r->pw = pw;

	r->refcnt = 1;

	/* Ref the lwc_string */
	lwc_context_ref(ctx);
	lwc_context_string_ref(ctx, str);

	*ret = r;
	return DOM_NO_ERR;

}

/**
 * Make the dom_string be interned in the lwc_context
 *
 * \param str     The dom_string to be interned
 * \param ctx     The lwc_context to intern this dom_string
 * \param lwcstr  The result lwc_string	
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_string_intern(struct dom_string *str, 
		struct lwc_context_s *ctx, struct lwc_string_s **lwcstr)
{
	lwc_string *ret;
	lwc_error lerr;

	/* If this string is interned with the same context, do nothing */
	if (str->context != NULL && str->context == ctx) {
		*lwcstr = str->intern;
		return DOM_NO_ERR;
	}

	lerr = lwc_context_intern(ctx, (const char *)str->ptr, str->len, &ret);
	if (lerr != lwc_error_ok) {
		return _dom_exception_from_lwc_error(lerr);
	}

	if (str->context != NULL) {
		lwc_context_unref(str->context);
		lwc_context_string_unref(str->context, str->intern);
		str->ptr = NULL;
	}

	str->context = ctx;
	str->intern = ret;
	lwc_context_ref(ctx);
	lwc_context_string_ref(ctx, ret);

	if (str->ptr != NULL) {
		str->alloc(str->ptr, 0, str->pw);
	}

	str->ptr = (uint8_t *) lwc_string_data(ret);

	*lwcstr = ret;
	return DOM_NO_ERR;
}

/**
 * Get the internal lwc_string 
 *
 * \param str     The dom_string object
 * \param ctx     The lwc_context which intern this dom_string
 * \param lwcstr  The lwc_string of this dom-string
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_string_get_intern(struct dom_string *str, 
		struct lwc_context_s **ctx, struct lwc_string_s **lwcstr)
{
	*ctx = str->context;
	*lwcstr = str->intern;

	if (*ctx != NULL)
		lwc_context_ref(*ctx);
	if (*lwcstr != NULL)
		lwc_context_string_ref(*ctx, *lwcstr);

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
	bool ret;

	if (s1 == NULL)
		s1 = &empty_string;

	if (s2 == NULL)
		s2 = &empty_string;

	if (s1->context == s2->context && s1->context != NULL) {
		assert(s1->intern != NULL);
		assert(s2->intern != NULL);
		lwc_context_string_isequal(s1->context, s1->intern, 
				s2->intern, &ret);
		if (ret == true) {
			return 0;
		} else {
			return -1;
		}
	}

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

	bool ret;
	if (s1->context == s2->context && s1->context != NULL) {
		assert(s1->intern != NULL);
		assert(s2->intern != NULL);
		lwc_context_string_caseless_isequal(s1->context, s1->intern, 
				s2->intern, &ret);
		if (ret == true) {
			return 0;
		} else {
			return -1;
		}
	}

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
	uint32_t c, coff, index;
	parserutils_error err;

	if (str == NULL)
		str = &empty_string;

	s = str->ptr;
	slen = str->len;

	index = dom_string_length(str);

	while (slen > 0) {
		err = parserutils_charset_utf8_prev(s, slen, 
				(uint32_t *) &coff);
		if (err == PARSERUTILS_OK) {
			err = parserutils_charset_utf8_to_ucs4(s + coff, 
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
 * Get the UCS4 character at position index
 *
 * \param index  The position of the charater
 * \param ch     The UCS4 character
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_string_at(struct dom_string *str, uint32_t index, 
		uint32_t *ch)
{
	const uint8_t *s;
	size_t clen, slen;
	uint32_t c, i;
	parserutils_error err;

	if (str == NULL)
		str = &empty_string;

	s = str->ptr;
	slen = str->len;

	i = 0;

	while (slen > 0) {
		err = parserutils_charset_utf8_char_byte_length(s, &clen);
		if (err != PARSERUTILS_OK) {
			return (uint32_t) -1;
		}

		i++;
		if (i == index + 1)
			break;

		s += clen;
		slen -= clen;
	}

	if (i == index + 1) {
		err = parserutils_charset_utf8_to_ucs4(s, slen, &c, &clen);
		if (err != PARSERUTILS_OK) {
			return (uint32_t) -1;
		}

		*ch = c;
		return DOM_NO_ERR;
	} else {
		return DOM_DOMSTRING_SIZE_ERR;
	}
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
	dom_alloc alloc;
	void *pw;

	assert(s1 != NULL);
	assert(s2 != NULL);

	if (s1->alloc != NULL) {
		alloc = s1->alloc;
		pw = s1->pw;
	} else if (s2->alloc != NULL) {
		alloc = s2->alloc;
		pw = s2->pw;
	} else {
		/* s1 == s2 == empty_string */
		*result = &empty_string;
		return DOM_NO_ERR;
	}

	concat = alloc(NULL, sizeof(struct dom_string), pw);

	if (concat == NULL) {
		return DOM_NO_MEM_ERR;
	}

	concat->ptr = alloc(NULL, s1->len + s2->len, pw);
	if (concat->ptr == NULL) {
		alloc(concat, 0, pw);

		return DOM_NO_MEM_ERR;
	}

	memcpy(concat->ptr, s1->ptr, s1->len);

	memcpy(concat->ptr + s1->len, s2->ptr, s2->len);

	concat->len = s1->len + s2->len;

	concat->alloc = alloc;
	concat->pw = pw;
	concat->context = NULL;
	concat->intern = NULL;

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
	uint32_t b1, b2;
	parserutils_error err;

	/* Initialise the byte index of the start to 0 */
	b1 = 0;
	/* Make the end a character offset from the start */
	i2 -= i1;

	/* Calculate the byte index of the start */
	while (i1 > 0) {
		err = parserutils_charset_utf8_next(s, slen, b1, &b1);
		if (err != PARSERUTILS_OK) {
			return DOM_NO_MEM_ERR;
		}

		i1--;
	}

	/* Initialise the byte index of the end to that of the start */
	b2 = b1;

	/* Calculate the byte index of the end */
	while (i2 > 0) {
		err = parserutils_charset_utf8_next(s, slen, b2, &b2);
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
			err = parserutils_charset_utf8_next(t, tlen, 
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
	res->intern = NULL;
	res->context = NULL;
	
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

	if (source == NULL)
		source = &empty_string;

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
		err = parserutils_charset_utf8_next(s, slen, b1, &b1);

		if (err != PARSERUTILS_OK) {
			return DOM_NO_MEM_ERR;
		}

		i1--;
	}

	/* Initialise the byte index of the end to that of the start */
	b2 = b1;

	/* Calculate the byte index of the end */
	while (i2 > 0) {
		err = parserutils_charset_utf8_next(s, slen, b2, &b2);

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
	res->intern = NULL;
	res->context = NULL;

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
	if (str->intern != NULL) {
		return _dom_string_create_from_lwcstring(str->alloc, str->pw,
				str->context, str->intern, result);
	} else {
		return dom_string_create(str->alloc, str->pw, str->ptr,
				str->len, result);
	}
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

/**
 * Convert a lwc_error to a dom_exception
 * 
 * \param err  The input lwc_error
 * \return the dom_exception
 */
dom_exception _dom_exception_from_lwc_error(lwc_error err)
{
	switch (err) {
		case lwc_error_ok:
			return DOM_NO_ERR;
		case lwc_error_oom:
			return DOM_NO_MEM_ERR;
		case lwc_error_range:
			return DOM_INDEX_SIZE_ERR;
	}
	assert ("Unknow lwc_error, can't convert to dom_exception");
	/* Suppress compile errors */
	return DOM_NO_ERR;
}

/**
 * Compare the raw data of two lwc_strings for equality when the two strings
 * belong to different lwc_context 
 * 
 * \param s1  The first lwc_string
 * \param s2  The second lwc_string
 * \return 0 for equal, non-zero otherwise
 */
int _dom_lwc_string_compare_raw(struct lwc_string_s *s1,
		struct lwc_string_s *s2)
{
	const char *rs1, *rs2;
	size_t len;

	if (lwc_string_length(s1) != lwc_string_length(s2))
		return -1;
	
	len = lwc_string_length(s1);
	rs1 = lwc_string_data(s1);
	rs2 = lwc_string_data(s2);

	return memcmp(rs1, rs2, len);
}

