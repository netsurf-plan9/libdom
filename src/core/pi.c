/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include "core/document.h"
#include "core/node.h"
#include "core/pi.h"

#include "utils/utils.h"

/**
 * A DOM processing instruction
 */
struct dom_processing_instruction {
	struct dom_node_internal base;		/**< Base node */
};

static struct dom_node_vtable pi_vtable = {
	DOM_NODE_VTABLE
};

static struct dom_node_protect_vtable pi_protect_vtable = {
	DOM_PI_PROTECT_VTABLE
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
dom_exception _dom_processing_instruction_create(struct dom_document *doc,
		struct lwc_string_s *name, dom_string *value,
		struct dom_processing_instruction **result)
{
	struct dom_processing_instruction *p;
	dom_exception err;

	/* Allocate the comment node */
	p = _dom_document_alloc(doc, NULL,
			sizeof(struct dom_processing_instruction));
	if (p == NULL)
		return DOM_NO_MEM_ERR;
	
	p->base.base.vtable = &pi_vtable;
	p->base.vtable = &pi_protect_vtable;

	/* And initialise the node */
	err = _dom_processing_instruction_initialise(&p->base, doc,
			DOM_PROCESSING_INSTRUCTION_NODE,
			name, value, NULL, NULL);
	if (err != DOM_NO_ERR) {
		_dom_document_alloc(doc, p, 0);
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
void _dom_processing_instruction_destroy(struct dom_document *doc,
		struct dom_processing_instruction *pi)
{
	/* Finalise base class */
	_dom_processing_instruction_finalise(doc, &pi->base);

	/* Free processing instruction */
	_dom_document_alloc(doc, pi, 0);
}

/*-----------------------------------------------------------------------*/

/* Following comes the protected vtable  */

/* The virtual destroy function of this class */
void _dom_pi_destroy(struct dom_node_internal *node)
{
	_dom_processing_instruction_destroy(node->owner, 
			(struct dom_processing_instruction *) node);
}

/* The memory allocator of this class */
dom_exception _dom_pi_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret)
{
	UNUSED(n);
	struct dom_processing_instruction *a;
	
	a = _dom_document_alloc(doc, NULL, 
			sizeof(struct dom_processing_instruction));
	if (a == NULL)
		return DOM_NO_MEM_ERR;
	
	*ret = (dom_node_internal *) a;
	dom_node_set_owner(*ret, doc);

	return DOM_NO_ERR;
}

/* The copy constructor of this class */
dom_exception _dom_pi_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old)
{
	return _dom_node_copy(new, old);
}

