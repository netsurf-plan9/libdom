/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *			http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include "resource_mgr.h"

#include <string.h>
#include <assert.h>

#include <libwapcaplet/libwapcaplet.h>
#include "core/string.h"

#include "utils/utils.h"

/**
 * Allocate some memory with this allocator
 *
 * \param res   The resource manager
 * \param size  The size of memory to allocate
 * \return the allocated memory pointer.
 */
void *_dom_resource_mgr_alloc(struct dom_resource_mgr *res, void *ptr, 
		size_t size)
{
	return res->alloc(ptr, size, res->pw);
}

/**
 * Create a dom_string using this resource manager
 *
 * \param res     The resource manager
 * \param data    The data pointer
 * \param len     The length of data
 * \param result  The returned dom_string
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_resource_mgr_create_string(struct dom_resource_mgr *res,
		const uint8_t *data, size_t len, dom_string **result)
{
	return dom_string_create(res->alloc, res->pw, data, len, result);
}

/**
 * Create a lwc_string using this resource manager
 *
 * \param res     The resource manager
 * \param data    The data pointer
 * \param len     The length of the data
 * \param result  The returned lwc_string
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_resource_mgr_create_lwcstring(struct dom_resource_mgr *res,
		const uint8_t *data, size_t len, struct lwc_string_s **result)
{
	lwc_error lerr;

	UNUSED(res);

	lerr = lwc_intern_string((const char *) data, len, result);
	
	return _dom_exception_from_lwc_error(lerr);
}

/**
 * Create a dom_string from a lwc_string using this resource manager
 *
 * \param res     The resource manager
 * \param str     The dom_string to intern
 * \param result  The returned lwc_string
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_resource_mgr_create_string_from_lwcstring(
		struct dom_resource_mgr *res, struct lwc_string_s *str, 
		dom_string **result)
{
	return _dom_string_create_from_lwcstring(res->alloc, res->pw, 
				str, result);
}

/**
 * Create a hash table using this resource manager
 * 
 * \param res     The resource manager
 * \param chains  The number of buckets of the hash table
 * \param f       The hash function 
 * \param ht      The returned hash table
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_resource_mgr_create_hashtable(struct dom_resource_mgr *res,
		size_t chains, dom_hash_func f, struct dom_hash_table **ht)
{
	struct dom_hash_table *ret;

	ret = _dom_hash_create(chains, f, res->alloc, res->pw);
	if (ret == NULL)
		return DOM_NO_MEM_ERR;
	
	*ht = ret;
	return DOM_NO_ERR;
}

