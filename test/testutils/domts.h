/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef domts_h_
#define domts_h_

#include <domtscondition.h>
#include <domtsasserts.h>
#include <list.h>
#include <foreach.h>
#include <utils.h>
#include <comparators.h>

struct lwc_context_s;

extern struct lwc_context_s *ctx;

dom_document *load_xml(char *file, bool willBeModified);
dom_document *load_html(char *file, bool willBeModified);

#endif
