/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_document_h_
#define dom_internal_document_h_

#include <inttypes.h>
#include <stddef.h>

struct dom_document;

const uint8_t *dom_document_get_base(struct dom_document *doc);
void *dom_document_alloc(struct dom_document *doc, void *ptr, size_t size);

#endif
