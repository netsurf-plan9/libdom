/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>

#include "html/html_document.h"
#include "html/html_select_element.h"

#include "core/node.h"
#include "utils/utils.h"

static struct dom_element_protected_vtable _protect_vtable = {
	{
		DOM_NODE_PROTECT_VTABLE_HTML_SELECT_ELEMENT
	},
	DOM_HTML_SELECT_ELEMENT_PROTECT_VTABLE
};

static bool is_option(struct dom_node_internal *node);

/**
 * Create a dom_html_select_element object
 *
 * \param doc  The document object
 * \param ele  The returned element object
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_html_select_element_create(struct dom_html_document *doc,
		struct dom_html_select_element **ele)
{
	struct dom_node_internal *node;

	*ele = malloc(sizeof(dom_html_select_element));
	if (*ele == NULL)
		return DOM_NO_MEM_ERR;
	
	/* Set up vtables */
	node = (struct dom_node_internal *) *ele;
	node->base.vtable = &_dom_element_vtable;
	node->vtable = &_protect_vtable;

	return _dom_html_select_element_initialise(doc, *ele);
}

/**
 * Initialise a dom_html_select_element object
 *
 * \param doc  The document object
 * \param ele  The dom_html_select_element object
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_html_select_element_initialise(struct dom_html_document *doc,
		struct dom_html_select_element *ele)
{
	dom_string *name = NULL;
	dom_exception err;

	err = dom_string_create((const uint8_t *) "SELECT", SLEN("SELECT"),
			&name);
	if (err != DOM_NO_ERR)
		return err;
	
	err = _dom_html_element_initialise(doc, &ele->base, name, NULL, NULL);
	dom_string_unref(name);

	ele->selected = -1;
	ele->options = NULL;

	return err;
}

/**
 * Finalise a dom_html_select_element object
 *
 * \param ele  The dom_html_select_element object
 */
void _dom_html_select_element_finalise(struct dom_html_select_element *ele)
{
	_dom_html_element_finalise(&ele->base);
}

/**
 * Destroy a dom_html_select_element object
 *
 * \param ele  The dom_html_select_element object
 */
void _dom_html_select_element_destroy(struct dom_html_select_element *ele)
{
	_dom_html_select_element_finalise(ele);
	free(ele);
}

/*------------------------------------------------------------------------*/
/* The protected virtual functions */

/* The virtual function used to parse attribute value, see src/core/element.c
 * for detail */
dom_exception _dom_html_select_element_parse_attribute(dom_element *ele,
		dom_string *name, dom_string *value,
		dom_string **parsed)
{
	UNUSED(ele);
	UNUSED(name);

	dom_string_ref(value);
	*parsed = value;

	return DOM_NO_ERR;
}

/* The virtual destroy function, see src/core/node.c for detail */
void _dom_virtual_html_select_element_destroy(dom_node_internal *node)
{
	_dom_html_select_element_destroy((struct dom_html_select_element *) node);
}

/* The virtual copy function, see src/core/node.c for detail */
dom_exception _dom_html_select_element_copy(dom_node_internal *old,
		dom_node_internal **copy)
{
	return _dom_html_element_copy(old, copy);
}

/*-----------------------------------------------------------------------*/
/* Public APIs */

/**
 * Get the ordinal index of the selected option
 *
 * \param ele    The element object
 * \param index  The returned index
 * \return DOM_NO_ERR on success.
 */
dom_exception dom_html_select_element_get_selected_index(
		dom_html_select_element *ele, unsigned long *index)
{
	*index = ele->selected;

	return DOM_NO_ERR;
}

/**
 * Set the ordinal index of the selected option
 *
 * \param ele    The element object
 * \param index  The new index
 * \return DOM_NO_ERR on success.
 */
dom_exception dom_html_select_element_set_selected_index(
		dom_html_select_element *ele, unsigned long index)
{
	ele->selected = index;

	return DOM_NO_ERR;
}

/**
 * Get the number of options in this select element
 *
 * \param ele  The element object
 * \param len  The returned len
 * \return DOM_NO_ERR on success.
 */
dom_exception dom_html_select_element_get_length(
		dom_html_select_element *ele, unsigned long *len)
{
	dom_exception err;
	dom_html_document *doc = (dom_html_document *) dom_node_get_owner(ele);
	assert(doc != NULL);

	if (ele->options == NULL) {
		err = _dom_html_options_collection_create(doc,
				(dom_node_internal *) ele,
				is_option, &ele->options);
		if (err != DOM_NO_ERR)
			return err;
	}

	return dom_html_options_collection_get_length(ele->options, len);
}

/**
 * Set the number of options in this select element
 *
 * \param ele  The element object
 * \param len  The new len
 * \return DOM_NOT_SUPPORTED_ERR.
 *
 * todo: how to deal with set the len of the children option objects?
 */
dom_exception dom_html_select_element_set_length(
		dom_html_select_element *ele, unsigned long len)
{
	UNUSED(ele);
	UNUSED(len);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * The collection of OPTION elements of this element
 *
 * \param ele  The element object
 * \param col  THe returned collection object
 * \return DOM_NO_ERR on success.
 */
dom_exception dom_html_select_element_get_options(
		dom_html_select_element *ele,
		struct dom_html_options_collection **col)
{
	dom_exception err;
	dom_html_document *doc = (dom_html_document *) dom_node_get_owner(ele);
	assert(doc != NULL);

	if (ele->options == NULL) {
		err = _dom_html_options_collection_create(doc,
				(dom_node_internal *) ele,
				is_option, &ele->options);
		if (err != DOM_NO_ERR)
			return err;

		*col = ele->options;
		return DOM_NO_ERR;
	}

	dom_html_options_collection_ref(ele->options);
	*col = ele->options;

	return DOM_NO_ERR;
}

/**
 * Whether this element is disabled
 *
 * \param ele       The element object
 * \param disabled  The returned status
 * \return DOM_NO_ERR on success.
 */
dom_exception dom_html_select_element_get_disabled(
		dom_html_select_element *ele, bool *disabled)
{
	return dom_html_element_get_bool_property(&ele->base,
			"disabled", SLEN("disabled"), disabled);
}

/**
 * Set the disabled status of this element
 *
 * \param ele       The element object
 * \param disabled  The disabled status
 * \return DOM_NO_ERR on success.
 */
dom_exception dom_html_select_element_set_disabled(
		dom_html_select_element *ele, bool disabled)
{
	return dom_html_element_set_bool_property(&ele->base,
			"disabled", SLEN("disabled"), disabled);
}

/**
 * Whether this element can be multiple selected
 *
 * \param ele       The element object
 * \param multiple  The returned status
 * \return DOM_NO_ERR on success.
 */
dom_exception dom_html_select_element_get_multiple(
		dom_html_select_element *ele, bool *multiple)
{
	return dom_html_element_get_bool_property(&ele->base,
			"multiple", SLEN("multiple"), multiple);
}

/**
 * Set whether this element can be multiple selected
 *
 * \param ele       The element object
 * \param multiple  The status
 * \return DOM_NO_ERR on success.
 */
dom_exception dom_html_select_element_set_multiple(
		dom_html_select_element *ele, bool multiple)
{
	return dom_html_element_set_bool_property(&ele->base,
			"multiple", SLEN("multiple"), multiple);
}

dom_exception dom_html_select_element_get_size(
		dom_html_select_element *ele, unsigned long *size);
dom_exception dom_html_select_element_set_size(
		dom_html_select_element *ele, unsigned long size);
dom_exception dom_html_select_element_get_tab_index(
		dom_html_select_element *ele, unsigned long *tab_index);
dom_exception dom_html_select_element_set_tab_index(
		dom_html_select_element *ele, unsigned long tab_index);

/* Functions */
dom_exception dom_html_select_element_add(struct dom_html_element *ele,
		struct dom_html_element *before);
dom_exception dom_html_element_blur(struct dom_html_select_element *ele);
dom_exception dom_html_element_focus(struct dom_html_select_element *ele);


/*-----------------------------------------------------------------------*/
/* Helper functions */

/* Test whether certain node is an option node */
bool is_option(struct dom_node_internal *node)
{
	dom_string *name = NULL;
	bool ret = false;
	dom_exception err;

	err = dom_string_create((const uint8_t *) "OPTION", SLEN("OPTION"),
			&name);
	if (err != DOM_NO_ERR)
		return false;

	if (dom_string_isequal(name, node->name))
		ret = true;
	
	dom_string_unref(name);

	return ret;
}
