/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com> 
 */

#include "core/cdatasection.h"
#include "core/document.h"
#include "core/text.h"
#include "utils/utils.h"

/**
 * A DOM CDATA section
 */
struct dom_cdata_section {
	struct dom_text base;		/**< Base node */
};

static struct dom_node_protect_vtable cdata_section_protect_vtable = {
	DOM_CDATA_SECTION_PROTECT_VTABLE
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
dom_exception _dom_cdata_section_create(struct dom_document *doc,
		struct lwc_string_s *name, struct dom_string *value,
		struct dom_cdata_section **result)
{
	struct dom_cdata_section *c;
	dom_exception err;

	/* Allocate the comment node */
	c = _dom_document_alloc(doc, NULL, sizeof(struct dom_cdata_section));
	if (c == NULL)
		return DOM_NO_MEM_ERR;
	
	/* Set up vtable */
	((dom_node_internal *) c)->base.vtable = &text_vtable;
	((dom_node_internal *) c)->vtable = &cdata_section_protect_vtable;

	/* And initialise the node */
	err = _dom_cdata_section_initialise(&c->base, doc,
			DOM_CDATA_SECTION_NODE, name, value);
	if (err != DOM_NO_ERR) {
		_dom_document_alloc(doc, c, 0);
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
void _dom_cdata_section_destroy(struct dom_document *doc,
		struct dom_cdata_section *cdata)
{
	/* Clean up base node contents */
	_dom_cdata_section_finalise(doc, &cdata->base);

	/* Destroy the node */
	_dom_document_alloc(doc, cdata, 0);
}

/*--------------------------------------------------------------------------*/

/* The protected virtual functions */

/* The virtual destroy function of this class */
void __dom_cdata_section_destroy(struct dom_node_internal *node)
{
	struct dom_document *doc;
	doc = dom_node_get_owner(node);

	_dom_cdata_section_destroy(doc, (struct dom_cdata_section *) node);
}

/* The memory allocator of this class */
dom_exception _dom_cdata_section_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret)
{
	UNUSED(n);
	dom_cdata_section *a;
	
	a = _dom_document_alloc(doc, NULL, sizeof(struct dom_cdata_section));
	if (a == NULL)
		return DOM_NO_MEM_ERR;
	
	*ret = (dom_node_internal *) a;
	dom_node_set_owner(*ret, doc);

	return DOM_NO_ERR;
}

/* The copy constructor of this class */
dom_exception _dom_cdata_section_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old)
{
	return _dom_characterdata_copy(new, old);
}

