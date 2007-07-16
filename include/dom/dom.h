/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

/** \file
 * This is the top-level header file for libdom. The intention of this is
 * to allow client applications to simply include this file and get access
 * to all the libdom API.
 */

#ifndef dom_dom_h_
#define dom_dom_h_

/* Base library headers */
#include <dom/functypes.h>

/* DOM bootstrap headers */
#include <dom/bootstrap/implregistry.h>

/* DOM core headers */
#include <dom/core/attr.h>
#include <dom/core/characterdata.h>
#include <dom/core/document.h>
#include <dom/core/document_type.h>
#include <dom/core/element.h>
#include <dom/core/exceptions.h>
#include <dom/core/implementation.h>
#include <dom/core/impllist.h>
#include <dom/core/namednodemap.h>
#include <dom/core/node.h>
#include <dom/core/nodelist.h>
#include <dom/core/string.h>
#include <dom/core/text.h>

#endif
