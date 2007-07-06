/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <inttypes.h>
#include <string.h>

#include <dom/ctx.h>
#include <dom/core/string.h>

#include "core/document.h"
#include "utils/utils.h"

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
	       DOM_STRING_OFFSET
	} type;
	union {
		uint8_t *ptr;
		const uint8_t *cptr;
		struct {
			struct dom_document *doc;
			uint32_t off;
		} offset;
	} data;

	size_t len;

	uint32_t refcnt;
};

/**
 * Claim a reference on a DOM string
 *
 * \param ctx  The context in which the string resides
 * \param str  The string to claim a reference on
 */
void dom_string_ref(struct dom_ctx *ctx, struct dom_string *str)
{
	UNUSED(ctx);

	str->refcnt++;
}

/**
 * Release a reference on a DOM string
 *
 * \param ctx  The context in which the string resides
 * \param str  The string to release the reference from
 *
 * If the reference count reaches zero, any memory claimed by the
 * string will be released
 */
void dom_string_unref(struct dom_ctx *ctx, struct dom_string *str)
{
	if (--str->refcnt == 0) {
		if (str->type == DOM_STRING_PTR)
			ctx->alloc(str->data.ptr, 0, ctx->pw);

		ctx->alloc(str, 0, ctx->pw);
	}
}

/**
 * Create a DOM string from an offset into the document buffer
 *
 * \param ctx  The context in which the string resides
 * \param doc  The document in which the string resides
 * \param off  Offset from start of document buffer
 * \param len  Length, in bytes, of string
 * \param str  Pointer to location to receive pointer to new string
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned string will already be referenced, so there is no need
 * to explicitly reference it.
 */
dom_exception dom_string_create_from_off(struct dom_ctx *ctx,
		struct dom_document *doc, uint32_t off, size_t len,
		struct dom_string **str)
{
	struct dom_string *ret;

	ret = ctx->alloc(NULL, sizeof(struct dom_string), ctx->pw);
	if (ret == NULL)
		return DOM_NO_MEM_ERR;

	ret->type = DOM_STRING_OFFSET;
	ret->data.offset.doc = doc;
	ret->data.offset.off = off;
	ret->len = len;

	ret->refcnt = 1;

	*str = ret;

	return DOM_NO_ERR;
}

/**
 * Create a DOM string from a string of characters
 *
 * \param ctx  The context in which the string resides
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
dom_exception dom_string_create_from_ptr(struct dom_ctx *ctx,
		const uint8_t *ptr, size_t len, struct dom_string **str)
{
	struct dom_string *ret;

	ret = ctx->alloc(NULL, sizeof(struct dom_string), ctx->pw);
	if (ret == NULL)
		return DOM_NO_MEM_ERR;

	ret->data.ptr = ctx->alloc(NULL, len, ctx->pw);
	if (ret->data.ptr == NULL) {
		ctx->alloc(ret, 0, ctx->pw);
		return DOM_NO_MEM_ERR;
	}

	ret->type = DOM_STRING_PTR;

	memcpy(ret->data.ptr, ptr, len);

	ret->len = len;

	ret->refcnt = 1;

	*str = ret;

	return DOM_NO_ERR;
}

/**
 * Create a DOM string from a constant string of characters
 *
 * \param ctx  The context in which the string resides
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
dom_exception dom_string_create_from_const_ptr(struct dom_ctx *ctx,
		const uint8_t *ptr, size_t len, struct dom_string **str)
{
	struct dom_string *ret;

	ret = ctx->alloc(NULL, sizeof(struct dom_string), ctx->pw);
	if (ret == NULL)
		return DOM_NO_MEM_ERR;

	ret->data.cptr = ptr;

	ret->type = DOM_STRING_CONST_PTR;

	ret->len = len;

	ret->refcnt = 1;

	*str = ret;

	return DOM_NO_ERR;
}

/**
 * Get a pointer to the string of characters within a DOM string
 *
 * \param ctx   The context in which the string resides
 * \param str   Pointer to DOM string to retrieve pointer from
 * \param data  Pointer to location to receive data
 * \param len   Pointer to location to receive byte length of data
 * \return DOM_NO_ERR on success
 *
 * The caller must have previously claimed a reference on the DOM string.
 * The returned pointer must not be freed.
 */
dom_exception dom_string_get_data(struct dom_ctx *ctx,
		struct dom_string *str, const uint8_t **data, size_t *len)
{
	switch (str->type) {
	case DOM_STRING_PTR:
		*data = str->data.ptr;
		break;
	case DOM_STRING_CONST_PTR:
		*data = str->data.cptr;
		break;
	case DOM_STRING_OFFSET:
		*data = dom_document_get_base(ctx, str->data.offset.doc) +
				str->data.offset.off;
		break;
	}

	*len = str->len;

	return DOM_NO_ERR;
}
