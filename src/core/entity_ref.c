/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include "core/document.h"
#include "core/entity_ref.h"
#include "core/node.h"
#include "utils/utils.h"

/**
 * A DOM entity reference
 */
struct dom_entity_reference {
	struct dom_node_internal base;		/**< Base node */
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
			name, value, NULL, NULL);
	if (err != DOM_NO_ERR) {
		dom_document_alloc(doc, e, 0);
		return err;
	}

	*result = e;

	return DOM_NO_ERR;
}

/**
 * Destroy an entity reference
 *
 * \param doc     The owning document
 * \param entity  The entity reference to destroy
 *
 * The contents of ::entity will be destroyed and ::entity will be freed.
 */
void dom_entity_reference_destroy(struct dom_document *doc,
		struct dom_entity_reference *entity)
{
	struct dom_node_internal *c, *d;

	/* Destroy children of this node */
	for (c = entity->base.first_child; c != NULL; c = d) {
		d = c->next;

		/* Detach child */
		c->parent = NULL;

		if (c->refcnt > 0) {
			/* Something is using this child */

			/** \todo add to list of nodes pending deletion */

			continue;
		}

		/* Detach from sibling list */
		c->previous = NULL;
		c->next = NULL;

		dom_node_destroy(c);
	}

	/* Finalise base class */
	dom_node_finalise(doc, &entity->base);

	/* Destroy fragment */
	dom_document_alloc(doc, entity, 0);
}

/**
 * Get the textual representation of an EntityReference
 *
 * \param entity  The entity reference to get the textual representation of
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_entity_reference_get_textual_representation(
		struct dom_entity_reference *entity, struct dom_string **result)
{
	UNUSED(entity);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

