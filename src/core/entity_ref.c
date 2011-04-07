/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <libwapcaplet/libwapcaplet.h>

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

static struct dom_node_vtable er_vtable = {
	DOM_NODE_VTABLE
};

static struct dom_node_protect_vtable er_protect_vtable = {
	DOM_ER_PROTECT_VTABLE
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
dom_exception _dom_entity_reference_create(struct dom_document *doc,
		struct lwc_string_s *name, dom_string *value,
		struct dom_entity_reference **result)
{
	struct dom_entity_reference *e;
	dom_exception err;

	/* Allocate the comment node */
	e = _dom_document_alloc(doc, NULL,
			sizeof(struct dom_entity_reference));
	if (e == NULL)
		return DOM_NO_MEM_ERR;

	e->base.base.vtable = &er_vtable;
	e->base.vtable = &er_protect_vtable;

	/* And initialise the node */
	err = _dom_entity_reference_initialise(&e->base, doc, 
			DOM_ENTITY_REFERENCE_NODE, name, value, NULL, NULL);
	if (err != DOM_NO_ERR) {
		_dom_document_alloc(doc, e, 0);
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
void _dom_entity_reference_destroy(struct dom_document *doc,
		struct dom_entity_reference *entity)
{
	/* Finalise base class */
	_dom_entity_reference_finalise(doc, &entity->base);

	/* Destroy fragment */
	_dom_document_alloc(doc, entity, 0);
}

/**
 * Get the textual representation of an EntityRererence
 *
 * \param entity  The entity reference to get the textual representation of
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR on success.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unrer the string once it has
 * finished with it.
 */
dom_exception _dom_entity_reference_get_textual_representation(
		struct dom_entity_reference *entity, dom_string **result)
{
	UNUSED(entity);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/*-----------------------------------------------------------------------*/

/* Following comes the protected vtable  */

/* The virtual destroy function of this class */
void _dom_er_destroy(struct dom_node_internal *node)
{
	_dom_entity_reference_destroy(node->owner, 
			(struct dom_entity_reference *) node);
}

/* The memory allocator of this class */
dom_exception _dom_er_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret)
{
	UNUSED(n);
	dom_entity_reference *a;
	
	a = _dom_document_alloc(doc, NULL, sizeof(struct dom_entity_reference));
	if (a == NULL)
		return DOM_NO_MEM_ERR;
	
	*ret = (dom_node_internal *) a;
	dom_node_set_owner(*ret, doc);

	return DOM_NO_ERR;
	
}

/* The copy constructor of this class */
dom_exception _dom_er_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old)
{
	return _dom_node_copy(new, old);
}

