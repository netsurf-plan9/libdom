/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include "core/characterdata.h"
#include "core/comment.h"
#include "core/document.h"

#include "utils/utils.h"

/**
 * A DOM Comment node
 */
struct dom_comment {
	struct dom_characterdata base;	/**< Base node */
};

static struct dom_node_protect_vtable comment_protect_vtable = {
	DOM_COMMENT_PROTECT_VTABLE
};

/**
 * Create a comment node
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
dom_exception _dom_comment_create(struct dom_document *doc,
		struct lwc_string_s *name, dom_string *value,
		struct dom_comment **result)
{
	struct dom_comment *c;
	dom_exception err;

	/* Allocate the comment node */
	c = _dom_document_alloc(doc, NULL, sizeof(struct dom_comment));
	if (c == NULL)
		return DOM_NO_MEM_ERR;

	/* Set the virtual table */
	((dom_node_internal *) c)->base.vtable = &characterdata_vtable;
	((dom_node_internal *) c)->vtable = &comment_protect_vtable;

	/* And initialise the node */
	err = _dom_characterdata_initialise(&c->base, doc, DOM_COMMENT_NODE,
			name, value);
	if (err != DOM_NO_ERR) {
		_dom_document_alloc(doc, c, 0);
		return err;
	}

	*result = c;

	return DOM_NO_ERR;
}

/**
 * Destroy a comment node
 *
 * \param doc      The owning document
 * \param comment  The node to destroy
 *
 * The contents of ::comment will be destroyed and ::comment will be freed
 */
void _dom_comment_destroy(struct dom_document *doc,
		struct dom_comment *comment)
{
	/* Finalise base class contents */
	_dom_characterdata_finalise(doc, &comment->base);

	/* Free node */
	_dom_document_alloc(doc, comment, 0);
}


/*-----------------------------------------------------------------------*/
/* The protected virtual functions */

/* The virtual destroy function */
void __dom_comment_destroy(struct dom_node_internal *node)
{
	struct dom_document *doc;
	doc = dom_node_get_owner(node);

	_dom_comment_destroy(doc, (struct dom_comment *) node);
}

/* The memory allocation function of this class */
dom_exception _dom_comment_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret)
{
	UNUSED(n);
	dom_comment *c;
	
	c = _dom_document_alloc(doc, NULL, sizeof(struct dom_comment));
	if (c == NULL)
		return DOM_NO_MEM_ERR;
	
	*ret = (dom_node_internal *) c;
	dom_node_set_owner(*ret, doc);

	return DOM_NO_ERR;
}

/* The copy constructor of this class */
dom_exception _dom_comment_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old)
{
	return _dom_characterdata_copy(new, old);
}

