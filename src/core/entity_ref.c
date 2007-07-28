/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include "core/document.h"
#include "core/entity_ref.h"
#include "core/node.h"

/**
 * A DOM entity reference
 */
struct dom_entity_reference {
	struct dom_node base;		/**< Base node */
};

/**
 * Create an entity reference
 *
 * \param doc     The owning document
 * \param name    The name of the node to create
 * \param value   The text content of the node
 * \param result  Pointer to location to receive created node
 * \return DOM_NO_ERR                on success,
 *         DOM_NO_MEM_ERR            on memory exhaustion.
 *
 * ::doc, ::name and ::value will have their reference counts increased.
 *
 * The returned node will already be referenced.
 */
dom_exception dom_entity_reference_create(struct dom_document *doc,
		struct dom_string *name, struct dom_string *value,
		struct dom_entity_reference **result)
{
	struct dom_entity_reference *e;
	dom_exception err;

	/* Allocate the comment node */
	e = dom_document_alloc(doc, NULL,
			sizeof(struct dom_entity_reference));
	if (e == NULL)
		return DOM_NO_MEM_ERR;

	/* And initialise the node */
	err = dom_node_initialise(&e->base, doc, DOM_ENTITY_REFERENCE_NODE,
			name, value);
	if (err != DOM_NO_ERR) {
		dom_document_alloc(doc, e, 0);
		return err;
	}

	*result = e;

	return DOM_NO_ERR;
}
