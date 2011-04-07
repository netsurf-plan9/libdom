/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_processinginstruction_h_
#define dom_internal_core_processinginstruction_h_

#include <dom/core/exceptions.h>

struct dom_document;
struct dom_processing_instruction;
struct lwc_string_s;

dom_exception _dom_processing_instruction_create(struct dom_document *doc,
		struct lwc_string_s *name, dom_string *value,
		struct dom_processing_instruction **result);

void _dom_processing_instruction_destroy(struct dom_document *doc,
		struct dom_processing_instruction *pi);

#define _dom_processing_instruction_initialise	_dom_node_initialise
#define _dom_processing_instruction_finalise 	_dom_node_finalise

/* Following comes the protected vtable  */
void _dom_pi_destroy(struct dom_node_internal *node);
dom_exception _dom_pi_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret);
dom_exception _dom_pi_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old);

#define DOM_PI_PROTECT_VTABLE \
	_dom_pi_destroy, \
	_dom_pi_alloc, \
	_dom_pi_copy

#endif
