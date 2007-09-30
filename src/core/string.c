/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <ctype.h>
#include <inttypes.h>
#include <string.h>

#include <dom/core/string.h>

#include "core/document.h"
#include "utils/utils.h"
#include "utils/utf8.h"
#include "utils/utf16.h"

/**
 * A DOM string
 *
 * DOM strings store either a pointer to allocated data, a pointer
 * to constant data or an offset into a document buffer.
 *
 * They are reference counted so freeing is performed correctly.
 */
struct dom_string {
	enum { DOM_STRING_PTR,
	       DOM_STRING_CONST_PTR,
	       DOM_STRING_OFFSET,
	       DOM_STRING_PTR_NODOC
	} type;				/**< String type */

	dom_string_charset charset;	/**< Charset of string */

	union {
		uint8_t *ptr;
		const uint8_t *cptr;
		uint32_t offset;
	} data;				/**< Type-specific data */

	size_t len;			/**< Byte length of string */

	union {
		struct dom_document *doc;	/**< Owning document */
		struct {
			dom_alloc alloc;	/**< Memory (de)allocation
						 * function */
			void *pw;	/**< Client-specific data */
		} nodoc;
	} ctx;				/**< Allocation context */

	uint32_t refcnt;		/**< Reference count */
};

static struct dom_string empty_string = { 
	.type = DOM_STRING_CONST_PTR,
	.charset = DOM_STRING_UTF8,
	.data.ptr = NULL,
	.len = 0,
	.ctx.doc = NULL,
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
		if (str->type == DOM_STRING_PTR_NODOC) {
			str->ctx.nodoc.alloc(str->data.ptr, 0,
					str->ctx.nodoc.pw);

			str->ctx.nodoc.alloc(str, 0, str->ctx.nodoc.pw);
		} else {
			if (str->type == DOM_STRING_PTR) {
				dom_document_alloc(str->ctx.doc,
						str->data.ptr, 0);
			}

			dom_document_alloc(str->ctx.doc, str, 0);
		}
	}
}

/**
 * Create a DOM string from an offset into the document buffer
 *
 * \param doc  The document in which the string resides
 * \param off  Offset from start of document buffer
 * \param len  Length, in bytes, of string
 * \param str  Pointer to location to receive pointer to new string
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will already be referenced, so there is no need
 * to explicitly reference it.
 */
dom_exception dom_string_create_from_off(struct dom_document *doc,
		uint32_t off, size_t len, struct dom_string **str)
{
	struct dom_string *ret;

	ret = dom_document_alloc(doc, NULL, sizeof(struct dom_string));
	if (ret == NULL)
		return DOM_NO_MEM_ERR;

	ret->type = DOM_STRING_OFFSET;

	ret->charset = dom_document_get_charset(doc);

	ret->data.offset = off;

	ret->len = len;

	ret->ctx.doc = doc;

	ret->refcnt = 1;

	*str = ret;

	return DOM_NO_ERR;
}

/**
 * Create a DOM string from a string of characters
 *
 * \param doc  The document in which the string resides
 * \param ptr  Pointer to string of characters
 * \param len  Length, in bytes, of string of characters
 * \param str  Pointer to location to receive pointer to new string
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will already be referenced, so there is no need
 * to explicitly reference it.
 *
 * The string of characters passed in will be copied for use by the
 * returned DOM string.
 */
dom_exception dom_string_create_from_ptr(struct dom_document *doc,
		const uint8_t *ptr, size_t len, struct dom_string **str)
{
	struct dom_string *ret;

	ret = dom_document_alloc(doc, NULL, sizeof(struct dom_string));
	if (ret == NULL)
		return DOM_NO_MEM_ERR;

	ret->data.ptr = dom_document_alloc(doc, NULL, len);
	if (ret->data.ptr == NULL) {
		dom_document_alloc(doc, ret, 0);
		return DOM_NO_MEM_ERR;
	}

	ret->type = DOM_STRING_PTR;

	ret->charset = dom_document_get_charset(doc);

	memcpy(ret->data.ptr, ptr, len);

	ret->len = len;

	ret->ctx.doc = doc;

	ret->refcnt = 1;

	*str = ret;

	return DOM_NO_ERR;
}

/**
 * Create a DOM string from a constant string of characters
 *
 * \param doc  The document in which the string resides
 * \param ptr  Pointer to string of characters
 * \param len  Length, in bytes, of string of characters
 * \param str  Pointer to location to receive pointer to new string
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will already be referenced, so there is no need
 * to explicitly reference it.
 *
 * The string of characters passed in will _not_ be copied for use by the
 * returned DOM string.
 */
dom_exception dom_string_create_from_const_ptr(struct dom_document *doc,
		const uint8_t *ptr, size_t len, struct dom_string **str)
{
	struct dom_string *ret;

	ret = dom_document_alloc(doc, NULL, sizeof(struct dom_string));
	if (ret == NULL)
		return DOM_NO_MEM_ERR;

	ret->type = DOM_STRING_CONST_PTR;

	ret->charset = dom_document_get_charset(doc);

	ret->data.cptr = ptr;

	ret->len = len;

	ret->ctx.doc = doc;

	ret->refcnt = 1;

	*str = ret;

	return DOM_NO_ERR;
}

/**
 * Create a DOM string from a string of characters that does not belong
 * to a document
 *
 * \param alloc    Memory (de)allocation function
 * \param pw       Pointer to client-specific private data
 * \param charset  The charset of the string
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
dom_exception dom_string_create_from_ptr_no_doc(dom_alloc alloc, void *pw,
		dom_string_charset charset, const uint8_t *ptr, size_t len, 
		struct dom_string **str)
{
	struct dom_string *ret;

	ret = alloc(NULL, sizeof(struct dom_string), pw);
	if (ret == NULL)
		return DOM_NO_MEM_ERR;

	ret->data.ptr = alloc(NULL, len, pw);
	if (ret->data.ptr == NULL) {
		alloc(ret, 0, pw);
		return DOM_NO_MEM_ERR;
	}

	ret->type = DOM_STRING_PTR_NODOC;

	ret->charset = charset;

	memcpy(ret->data.ptr, ptr, len);

	ret->len = len;

	ret->ctx.nodoc.alloc = alloc;
	ret->ctx.nodoc.pw = pw;

	ret->refcnt = 1;

	*str = ret;

	return DOM_NO_ERR;
}

/**
 * Get a pointer to the string of characters within a DOM string
 *
 * \param str   Pointer to DOM string to retrieve pointer from
 * \param data  Pointer to location to receive data
 * \param len   Pointer to location to receive byte length of data
 * \return DOM_NO_ERR on success
 *
 * The caller must have previously claimed a reference on the DOM string.
 * The returned pointer must not be freed.
 */
dom_exception dom_string_get_data(struct dom_string *str,
		const uint8_t **data, size_t *len)
{
	/* Assume that a NULL str pointer indicates the empty string */
	if (str == NULL)
		str = &empty_string;

	switch (str->type) {
	case DOM_STRING_PTR:
		*data = str->data.ptr;
		break;
	case DOM_STRING_CONST_PTR:
		*data = str->data.cptr;
		break;
	case DOM_STRING_OFFSET:
		*data = dom_document_get_base(str->ctx.doc) +
				str->data.offset;
		break;
	case DOM_STRING_PTR_NODOC:
		*data = str->data.ptr;
		break;
	}

	*len = str->len;

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
	const uint8_t *d1 = NULL;
	const uint8_t *d2 = NULL;
	size_t l1, l2;
	dom_exception err;

	err = dom_string_get_data(s1, &d1, &l1);
	if (err != DOM_NO_ERR)
		return 1; /* arbitrary */

	err = dom_string_get_data(s2, &d2, &l2);
	if (err != DOM_NO_ERR)
		return 1; /* arbitrary */

	while (l1 > 0 && l2 > 0) {
		uint32_t c1, c2;
		size_t cl1, cl2;
		charset_error err;

		err = (s1->charset == DOM_STRING_UTF8) 
				? _dom_utf8_to_ucs4(d1, l1, &c1, &cl1) 
				: _dom_utf16_to_ucs4(d1, l1, &c1, &cl1);
		if (err != CHARSET_OK) {
		}

		err = (s2->charset == DOM_STRING_UTF8)
				? _dom_utf8_to_ucs4(d2, l2, &c2, &cl2)
				: _dom_utf16_to_ucs4(d2, l2, &c2, &cl2);
		if (err != CHARSET_OK) {
		}

		if (c1 != c2) {
			return (int)(c1 - c2);
		}

		d1 += cl1;
		d2 += cl2;

		l1 -= cl1;
		l2 -= cl2;
	}

	return (int)(l1 - l2);
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
	dom_exception err;

	err = dom_string_get_data(s1, &d1, &l1);
	if (err != DOM_NO_ERR)
		return 1; /* arbitrary */

	err = dom_string_get_data(s2, &d2, &l2);
	if (err != DOM_NO_ERR)
		return 1; /* arbitrary */

	while (l1 > 0 && l2 > 0) {
		uint32_t c1, c2;
		size_t cl1, cl2;
		charset_error err;

		err = (s1->charset == DOM_STRING_UTF8) 
				? _dom_utf8_to_ucs4(d1, l1, &c1, &cl1) 
				: _dom_utf16_to_ucs4(d1, l1, &c1, &cl1);
		if (err != CHARSET_OK) {
		}

		err = (s2->charset == DOM_STRING_UTF8)
				? _dom_utf8_to_ucs4(d2, l2, &c2, &cl2)
				: _dom_utf16_to_ucs4(d2, l2, &c2, &cl2);
		if (err != CHARSET_OK) {
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

