/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *			http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_utils_resource_mgr_h_
#define dom_utils_resource_mgr_h_

#include <dom/functypes.h>
#include <dom/core/exceptions.h>

#include "hashtable.h"

struct lwc_string_s;
struct dom_string;

/**
 * Resource manager
 */
typedef struct dom_resource_mgr {
	dom_alloc alloc;
	void *pw;
} dom_resource_mgr;

void *_dom_resource_mgr_alloc(struct dom_resource_mgr *res, void *ptr, 
		size_t size);

dom_exception _dom_resource_mgr_create_string(struct dom_resource_mgr *res,
		const uint8_t *data, size_t len, struct dom_string **result);

dom_exception _dom_resource_mgr_create_lwcstring(struct dom_resource_mgr *res,
		const uint8_t *data, size_t len, struct lwc_string_s **result);

dom_exception _dom_resource_mgr_create_string_from_lwcstring(
		struct dom_resource_mgr *res, struct lwc_string_s *str, 
		struct dom_string **result);

dom_exception _dom_resource_mgr_create_hashtable(struct dom_resource_mgr *res,
		size_t chains, dom_hash_func f, struct dom_hash_table **ht);

#endif
