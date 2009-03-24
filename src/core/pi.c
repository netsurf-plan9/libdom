/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include "core/document.h"
#include "core/node.h"
#include "core/pi.h"

/**
 * A DOM processing instruction
 */
struct dom_processing_instruction {
	struct dom_node_internal base;		/**< Base node */
};

/**
 * Create a processing instruction
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
dom_exception dom_processing_instruction_create(struct dom_document *doc,
		struct dom_string *name, struct dom_string *value,
		struct dom_processing_instruction **result)
{
	struct dom_processing_instruction *p;
	dom_exception err;

	/* Allocate the comment node */
	p = dom_document_alloc(doc, NULL,
			sizeof(struct dom_processing_instruction));
	if (p == NULL)
		return DOM_NO_MEM_ERR;

	/* And initialise the node */
	err = dom_node_initialise(&p->base, doc,
			DOM_PROCESSING_INSTRUCTION_NODE,
			name, value, NULL, NULL);
	if (err != DOM_NO_ERR) {
		dom_document_alloc(doc, p, 0);
		return err;
	}

	*result = p;

	return DOM_NO_ERR;
}

/**
 * Destroy a processing instruction
 *
 * \param doc  The owning document
 * \param pi   The processing instruction to destroy
 *
 * The contents of ::pi will be destroyed and ::pi will be freed.
 */
void dom_processing_instruction_destroy(struct dom_document *doc,
		struct dom_processing_instruction *pi)
{
	/* Finalise base class */
	dom_node_finalise(doc, &pi->base);

	/* Free processing instruction */
	dom_document_alloc(doc, pi, 0);
}
