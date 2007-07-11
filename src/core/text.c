/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/core/string.h>
#include <dom/core/text.h>

#include "core/characterdata.h"
#include "utils/utils.h"

struct dom_text {
	struct dom_characterdata base;	/**< Base node */

	bool element_content_whitespace;	/**< This node is element
						 * content whitespace */
};

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
dom_exception dom_text_split_text(struct dom_text *text,
		unsigned long offset, struct dom_text **result)
{
	UNUSED(text);
	UNUSED(offset);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Determine if a text node contains element content whitespace
 *
 * \param text    The node to consider
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_text_get_is_element_content_whitespace(
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
dom_exception dom_text_get_whole_text(struct dom_text *text,
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
dom_exception dom_text_replace_whole_text(struct dom_text *text,
		struct dom_string *content, struct dom_text **result)
{
	UNUSED(text);
	UNUSED(content);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}
