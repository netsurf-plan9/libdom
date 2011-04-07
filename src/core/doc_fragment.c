/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <libwapcaplet/libwapcaplet.h>

#include <dom/core/node.h>

#include "core/document.h"
#include "core/doc_fragment.h"
#include "core/node.h"
#include "utils/utils.h"

/**
 * A DOM document fragment
 */
struct dom_document_fragment {
	struct dom_node_internal base;		/**< Base node */
};

static struct dom_node_vtable df_vtable = {
	DOM_NODE_VTABLE
};

static struct dom_node_protect_vtable df_protect_vtable = {
	DOM_DF_PROTECT_VTABLE
};

/**
 * Create a document fragment
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
dom_exception _dom_document_fragment_create(struct dom_document *doc,
		struct lwc_string_s *name, dom_string *value,
		struct dom_document_fragment **result)
{
	struct dom_document_fragment *f;
	dom_exception err;

	/* Allocate the comment node */
	f = _dom_document_alloc(doc, NULL,
			sizeof(struct dom_document_fragment));
	if (f == NULL)
		return DOM_NO_MEM_ERR;


	f->base.base.vtable = &df_vtable;
	f->base.vtable = &df_protect_vtable;

	/* And initialise the node */
	err = _dom_document_fragment_initialise(&f->base, doc, 
			DOM_DOCUMENT_FRAGMENT_NODE, name, value, NULL, NULL);
	if (err != DOM_NO_ERR) {
		_dom_document_alloc(doc, f, 0);
		return err;
	}

	*result = f;

	return DOM_NO_ERR;
}

/**
 * Destroy a document fragment
 *
 * \param doc   The owning document
 * \param frag  The document fragment to destroy
 *
 * The contents of ::frag will be destroyed and ::frag will be freed.
 */
void _dom_document_fragment_destroy(struct dom_document *doc,
		struct dom_document_fragment *frag)
{
	/* Finalise base class */
	_dom_document_fragment_finalise(doc, &frag->base);

	/* Destroy fragment */
	_dom_document_alloc(doc, frag, 0);
}

/*-----------------------------------------------------------------------*/

/* Overload protected functions */

/* The virtual destroy function of this class */
void _dom_df_destroy(struct dom_node_internal *node)
{
	_dom_document_fragment_destroy(node->owner,
			(struct dom_document_fragment *) node);
}

/* The memory allocator of this class */
dom_exception _dom_df_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret)
{
	UNUSED(n);
	struct dom_document_fragment *a;
	
	a = _dom_document_alloc(doc, NULL,
			sizeof(struct dom_document_fragment));
	if (a == NULL)
		return DOM_NO_MEM_ERR;
	
	*ret = (dom_node_internal *) a;
	dom_node_set_owner(*ret, doc);

	return DOM_NO_ERR;
}

/* The copy constructor of this class */
dom_exception _dom_df_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old)
{
	return _dom_node_copy(new, old);
}

