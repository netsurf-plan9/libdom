/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include "core/cdatasection.h"
#include "core/document.h"
#include "core/text.h"

/**
 * A DOM CDATA section
 */
struct dom_cdata_section {
	struct dom_text base;		/**< Base node */
};

/**
 * Create a CDATA section
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
dom_exception dom_cdata_section_create(struct dom_document *doc,
		struct dom_string *name, struct dom_string *value,
		struct dom_cdata_section **result)
{
	struct dom_cdata_section *c;
	dom_exception err;

	/* Allocate the comment node */
	c = dom_document_alloc(doc, NULL, sizeof(struct dom_cdata_section));
	if (c == NULL)
		return DOM_NO_MEM_ERR;

	/* And initialise the node */
	err = dom_text_initialise(&c->base, doc, DOM_CDATA_SECTION_NODE,
			name, value);
	if (err != DOM_NO_ERR) {
		dom_document_alloc(doc, c, 0);
		return err;
	}

	*result = c;

	return DOM_NO_ERR;
}

/**
 * Destroy a CDATA section
 *
 * \param doc    The owning document
 * \param cdata  The cdata section to destroy
 *
 * The contents of ::cdata will be destroyed and ::cdata will be freed.
 */
void dom_cdata_section_destroy(struct dom_document *doc,
		struct dom_cdata_section *cdata)
{
	/* Clean up base node contents */
	dom_text_finalise(doc, &cdata->base);

	/* Destroy the node */
	dom_document_alloc(doc, cdata, 0);
}
