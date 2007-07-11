/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_characterdata_h_
#define dom_core_characterdata_h_

#include <dom/core/exceptions.h>

struct dom_characterdata;
struct dom_string;

dom_exception dom_characterdata_get_data(struct dom_characterdata *cdata,
		struct dom_string **data);
dom_exception dom_characterdata_set_data(struct dom_characterdata *cdata,
		struct dom_string *data);
dom_exception dom_characterdata_get_length(struct dom_characterdata *cdata,
		unsigned long *length);
dom_exception dom_characterdata_substring_data(
		struct dom_characterdata *cdata, unsigned long offset,
		unsigned long count, struct dom_string **data);
dom_exception dom_characterdata_append_data(struct dom_characterdata *cdata,
		struct dom_string *data);
dom_exception dom_characterdata_insert_data(struct dom_characterdata *cdata,
		unsigned long offset, struct dom_string *data);
dom_exception dom_characterdata_delete_data(struct dom_characterdata *cdata,
		unsigned long offset, unsigned long count);
dom_exception dom_characterdata_replace_data(struct dom_characterdata *cdata,
		unsigned long offset, unsigned long count,
		struct dom_string *data);

#endif
