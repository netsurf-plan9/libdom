/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/functypes.h>
#include <dom/core/document_type.h>
#include "utils/utils.h"

dom_exception dom_document_type_get_name(struct dom_document_type *docType,
		struct dom_string **result)
{
	UNUSED(docType);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}
