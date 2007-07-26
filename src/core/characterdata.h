/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_characterdata_h_
#define dom_internal_core_characterdata_h_

#include "core/node.h"

/**
 * DOM character data node
 */
struct dom_characterdata {
	struct dom_node base;		/**< Base node */
};

dom_exception dom_characterdata_create(struct dom_document *doc,
		dom_node_type type, struct dom_string *name,
		struct dom_string *value, struct dom_characterdata **result);

dom_exception dom_characterdata_initialise(struct dom_characterdata *cdata,
		struct dom_document *doc, dom_node_type type,
		struct dom_string *name, struct dom_string *value);

#endif
