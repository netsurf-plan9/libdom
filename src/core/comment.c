/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include "core/characterdata.h"
#include "core/comment.h"
#include "core/document.h"

/**
 * A DOM comment node
 */
struct dom_comment {
	struct dom_characterdata base;	/**< Base node */
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
dom_exception dom_comment_create(struct dom_document *doc,
		struct dom_string *name, struct dom_string *value,
		struct dom_comment **result)
{
	struct dom_comment *c;
	dom_exception err;

	/* Allocate the comment node */
	c = dom_document_alloc(doc, NULL, sizeof(struct dom_comment));
	if (c == NULL)
		return DOM_NO_MEM_ERR;

	/* And initialise the node */
	err = dom_characterdata_initialise(&c->base, doc, DOM_COMMENT_NODE,
			name, value);
	if (err != DOM_NO_ERR) {
		dom_document_alloc(doc, c, 0);
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
void dom_comment_destroy(struct dom_document *doc,
		struct dom_comment *comment)
{
	/* Finalise base class contents */
	dom_characterdata_finalise(doc, &comment->base);

	/* Free node */
	dom_document_alloc(doc, comment, 0);
}
