/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_core_typeinfo_h_
#define dom_core_typeinfo_h_

#include <stdbool.h>

#include <dom/core/exceptions.h>

struct dom_string;

typedef struct dom_type_info dom_type_info;

typedef enum {
	DOM_TYPE_INFO_DERIVATION_RESTRICTION	= 0x00000001,
	DOM_TYPE_INFO_DERIVATION_EXTENSION		= 0x00000002,
	DOM_TYPE_INFO_DERIVATION_UNION			= 0x00000004,
	DOM_TYPE_INFO_DERIVATION_LIST			= 0x00000008
} dom_type_info_derivation_method;

dom_exception _dom_type_info_get_type_name(dom_type_info *ti, 
		struct dom_string **ret);
#define dom_type_info_get_type_name(t, r) _dom_type_info_get_type_name( \
		(dom_type_info *) (t), (struct dom_string **) (r))


dom_exception _dom_type_info_get_type_namespace(dom_type_info *ti,
		struct dom_string **ret);
#define dom_type_info_get_type_namespace(t, r) \
		_dom_type_info_get_type_namespace((dom_type_info *) (t),\
		(struct dom_string **) (r))


dom_exception _dom_type_info_is_derived(dom_type_info *ti,
		struct dom_string *namespace, struct dom_string *name, 
		dom_type_info_derivation_method method, bool *ret);
#define dom_type_info_is_derived(t, s, n, m, r)  _dom_type_info_is_derived(\
		(dom_type_info *) (t), (struct dom_string *) (s), \
		(struct dom_string *) (n), \
		(dom_type_info_derivation_method) (m), (bool *) (r))


#endif
