/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_entityrererence_h_
#define dom_internal_core_entityrererence_h_

#include <dom/core/exceptions.h>
#include <dom/core/entity_ref.h>

struct dom_document;
struct dom_entity_reference;
struct dom_string;
struct lwc_string_s;

dom_exception _dom_entity_reference_create(struct dom_document *doc,
		struct lwc_string_s *name, struct dom_string *value,
		struct dom_entity_reference **result);

void _dom_entity_reference_destroy(struct dom_document *doc,
		struct dom_entity_reference *entity);

#define _dom_entity_reference_initialise _dom_node_initialise
#define _dom_entity_reference_finalise	_dom_node_finalise

/* Following comes the protected vtable  */
void _dom_er_destroy(struct dom_node_internal *node);
dom_exception _dom_er_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret);
dom_exception _dom_er_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old);

#define DOM_ER_PROTECT_VTABLE \
	_dom_er_destroy, \
	_dom_er_alloc, \
	_dom_er_copy

/* Helper functions */
dom_exception _dom_entity_reference_get_textual_representation(
		struct dom_entity_reference *entity,
		struct dom_string **result);

#endif
