/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/core/string.h>
#include <dom/core/text.h>

#include "core/characterdata.h"
#include "core/document.h"
#include "core/text.h"
#include "utils/utils.h"

/* The virtual table for dom_text */
static struct dom_text_vtable text_vtable = {
	{
		{
			DOM_NODE_VTABLE
		},
		DOM_CHARACTERDATA_VTABLE
	},
	DOM_TEXT_VTABLE
};

/* The destroy virtual function */
void _dom_text_destroy(struct dom_node_internal *node);
void _dom_text_destroy(struct dom_node_internal *node)
{
	struct dom_document *doc;
	dom_node_get_owner_document(node, &doc);

	dom_text_destroy(doc, (struct dom_text *) node);
}

/**
 * Create a text node
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
dom_exception dom_text_create(struct dom_document *doc,
		struct dom_string *name, struct dom_string *value,
		struct dom_text **result)
{
	struct dom_text *t;
	dom_exception err;

	/* Allocate the text node */
	t = dom_document_alloc(doc, NULL, sizeof(struct dom_text));
	if (t == NULL)
		return DOM_NO_MEM_ERR;

	/* And initialise the node */
	err = dom_text_initialise(t, doc, DOM_TEXT_NODE, name, value);
	if (err != DOM_NO_ERR) {
		dom_document_alloc(doc, t, 0);
		return err;
	}

	*result = t;

	return DOM_NO_ERR;
}

/**
 * Destroy a text node
 *
 * \param doc   The owning document
 * \param text  The text node to destroy
 *
 * The contents of ::text will be destroyed and ::text will be freed.
 */
void dom_text_destroy(struct dom_document *doc, struct dom_text *text)
{
	/* Finalise node */
	dom_text_finalise(doc, text);

	/* Free node */
	dom_document_alloc(doc, text, 0);
}

/**
 * Initialise a text node
 *
 * \param text   The node to initialise
 * \param doc    The owning document
 * \param type   The type of the node
 * \param name   The name of the node to create
 * \param value  The text content of the node
 * \return DOM_NO_ERR on success.
 *
 * ::doc, ::name and ::value will have their reference counts increased.
 */
dom_exception dom_text_initialise(struct dom_text *text,
		struct dom_document *doc, dom_node_type type,
		struct dom_string *name, struct dom_string *value)
{
	dom_exception err;

	/* Initialise the base class */
	err = dom_characterdata_initialise(&text->base, doc, type,
			name, value);
	if (err != DOM_NO_ERR)
		return err;

	/* Compose the vtable */
	((struct dom_node *) text)->vtable = &text_vtable;
	text->base.base.destroy = &_dom_text_destroy;

	/* Perform our type-specific initialisation */
	text->element_content_whitespace = false;

	return DOM_NO_ERR;
}

/**
 * Finalise a text node
 *
 * \param doc   The owning document
 * \param text  The text node to finalise
 *
 * The contents of ::text will be cleaned up. ::text will not be freed.
 */
void dom_text_finalise(struct dom_document *doc, struct dom_text *text)
{
	dom_characterdata_finalise(doc, &text->base);
}

/**
 * Split a text node at a given character offset
 *
 * \param text  The node to split
 * \param offset  Character offset to split at
 * \param result  Pointer to location to receive new node
 * \return DOM_NO_ERR                      on success,
 *         DOM_INDEX_SIZE_ERR              if ::offset is greater than the
 *                                         number of characters in ::text,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::text is readonly.
 *
 * The returned node will be referenced. The client should unref the node
 * once it has finished with it.
 */
dom_exception _dom_text_split_text(struct dom_text *text,
		unsigned long offset, struct dom_text **result)
{
	struct dom_node_internal *t = (struct dom_node_internal *) text;
	struct dom_string *value;
	struct dom_text *res;
	unsigned long len;
	dom_exception err;

	if (_dom_node_readonly(t)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	err = dom_characterdata_get_length(&text->base, &len);
	if (err != DOM_NO_ERR) {
		return err;
	}

	if (offset >= len) {
		return DOM_INDEX_SIZE_ERR;
	}

	/* Retrieve the data after the split point -- 
	 * this will be the new node's value. */
	err = dom_characterdata_substring_data(&text->base, offset, 
			len - offset, &value);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Create new node */
	err = dom_text_create(t->owner, t->name, value, &res);
	if (err != DOM_NO_ERR) {
		dom_string_unref(value);
		return err;
	}

	/* Release our reference on the value (the new node has its own) */
	dom_string_unref(value);

	/* Delete the data after the split point */
	err = dom_characterdata_delete_data(&text->base, offset, len - offset);
	if (err != DOM_NO_ERR) {
		dom_node_unref(res);
		return err;
	}

	*result = res;

	return DOM_NO_ERR;
}

/**
 * Determine if a text node contains element content whitespace
 *
 * \param text    The node to consider
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_text_get_is_element_content_whitespace(
		struct dom_text *text, bool *result)
{
	*result = text->element_content_whitespace;

	return DOM_NO_ERR;
}

/**
 * Retrieve all text in Text nodes logically adjacent to a Text node
 *
 * \param text    Text node to consider
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception _dom_text_get_whole_text(struct dom_text *text,
		struct dom_string **result)
{
	UNUSED(text);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Replace the text of a Text node and all logically adjacent Text nodes
 *
 * \param text     Text node to consider
 * \param content  Replacement content
 * \param result   Pointer to location to receive Text node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if one of the Text nodes being
 *                                         replaced is readonly.
 *
 * The returned node will be referenced. The client should unref the node
 * once it has finished with it.
 */
dom_exception _dom_text_replace_whole_text(struct dom_text *text,
		struct dom_string *content, struct dom_text **result)
{
	UNUSED(text);
	UNUSED(content);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

