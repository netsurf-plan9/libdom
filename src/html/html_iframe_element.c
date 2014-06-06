/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 * Copyright 2014 Rupinder Singh Khokhar<rsk1coder99@gmail.com>
 */
#include <assert.h>
#include <stdlib.h>

#include <dom/html/html_iframe_element.h>

#include "html/html_document.h"
#include "html/html_iframe_element.h"

#include "core/node.h"
#include "core/attr.h"
#include "utils/utils.h"

static struct dom_element_protected_vtable _protect_vtable = {
	{
		DOM_NODE_PROTECT_VTABLE_HTML_IFRAME_ELEMENT
	},
	DOM_HTML_IFRAME_ELEMENT_PROTECT_VTABLE
};

/**
 * Create a dom_html_iframe_element object
 *
 * \param doc  The document object
 * \param ele  The returned element object
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_html_iframe_element_create(struct dom_html_document *doc,
		dom_string *namespace, dom_string *prefix,
		struct dom_html_iframe_element **ele)
{
	struct dom_node_internal *node;

	*ele = malloc(sizeof(dom_html_iframe_element));
	if (*ele == NULL)
		return DOM_NO_MEM_ERR;

	/* Set up vtables */
	node = (struct dom_node_internal *) *ele;
	node->base.vtable = &_dom_html_element_vtable;
	node->vtable = &_protect_vtable;

	return _dom_html_iframe_element_initialise(doc, namespace, prefix, *ele);
}

/**
 * Initialise a dom_html_iframe_element object
 *
 * \param doc  The document object
 * \param ele  The dom_html_iframe_element object
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_html_iframe_element_initialise(struct dom_html_document *doc,
		dom_string *namespace, dom_string *prefix,
		struct dom_html_iframe_element *ele)
{
        dom_string *scrolling_default = NULL;
        dom_exception err;
        err = dom_string_create((const uint8_t *) "auto", SLEN("auto"), &scrolling_default);
        if (err != DOM_NO_ERR)
               return err;


        dom_string *frame_border_default = NULL;
        err = dom_string_create((const uint8_t *) "1", SLEN("1"), &frame_border_default);
        if (err != DOM_NO_ERR)
               return err;

	err = _dom_html_element_initialise(doc, &ele->base,
					    doc->memoised[hds_IFRAME],
					    namespace, prefix);

	ele->scrolling_default = scrolling_default;
	ele->frame_border_default = frame_border_default;
	
	return err;
}

/**
 * Finalise a dom_html_iframe_element object
 *
 * \param ele  The dom_html_iframe_element object
 */
void _dom_html_iframe_element_finalise(struct dom_html_iframe_element *ele)
{
	_dom_html_element_finalise(&ele->base);
}

/**
 * Destroy a dom_html_iframe_element object
 *
 * \param ele  The dom_html_iframe_element object
 */
void _dom_html_iframe_element_destroy(struct dom_html_iframe_element *ele)
{
	_dom_html_iframe_element_finalise(ele);
	free(ele);
}

/*------------------------------------------------------------------------*/
/* The protected virtual functions */

/* The virtual function used to parse attribute value, see src/core/element.c
 * for detail */
dom_exception _dom_html_iframe_element_parse_attribute(dom_element *ele,
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
void _dom_virtual_html_iframe_element_destroy(dom_node_internal *node)
{
	_dom_html_iframe_element_destroy((struct dom_html_iframe_element *) node);
}

/* The virtual copy function, see src/core/node.c for detail */
dom_exception _dom_html_iframe_element_copy(dom_node_internal *old,
		                dom_node_internal **copy)
{
	        return _dom_html_element_copy(old, copy);
}

/*-----------------------------------------------------------------------*/
/* API functions */

#define SIMPLE_GET(attr)						\
	dom_exception dom_html_iframe_element_get_##attr(		\
		dom_html_iframe_element *element,			\
		dom_string **attr)					\
	{								\
		dom_exception ret;					\
		dom_string *_memo_##attr;				\
									\
		_memo_##attr =						\
			((struct dom_html_document *)			\
			 ((struct dom_node_internal *)element)->owner)->\
			memoised[hds_##attr];				\
									\
		ret = dom_element_get_attribute(element, _memo_##attr, attr); \
									\
		return ret;						\
	}
#define SIMPLE_SET(attr)						\
dom_exception dom_html_iframe_element_set_##attr(			\
		dom_html_iframe_element *element,			\
		dom_string *attr)					\
	{								\
		dom_exception ret;					\
		dom_string *_memo_##attr;				\
									\
		_memo_##attr =						\
			((struct dom_html_document *)			\
			 ((struct dom_node_internal *)element)->owner)->\
			memoised[hds_##attr];				\
									\
		ret = dom_element_set_attribute(element, _memo_##attr, attr); \
									\
		return ret;						\
	}

#define SIMPLE_GET_SET(attr) SIMPLE_GET(attr) SIMPLE_SET(attr)

SIMPLE_GET_SET(long_desc);
SIMPLE_GET_SET(align);
SIMPLE_GET_SET(name);
SIMPLE_GET_SET(src);
SIMPLE_GET_SET(margin_width);
SIMPLE_GET_SET(margin_height);
SIMPLE_SET(scrolling);
SIMPLE_SET(frame_border);
SIMPLE_GET_SET(width);
SIMPLE_GET_SET(height);


/**
 * Get the frame_border property
 *
 * \param ele		The dom_html_iframe_element object
 * \param iframe_border		The returned status
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_html_iframe_element_get_frame_border(
		dom_html_iframe_element *ele,
		dom_string **frame_border) 
{
	dom_html_document *doc;
        bool has_value = false;
        dom_exception err;

        doc = (dom_html_document *) ((dom_node_internal *) ele)->owner;

        err = dom_element_has_attribute(ele,
                         doc->memoised[hds_frame_border], &has_value);
        if(err !=DOM_NO_ERR)
                return err;

        if(has_value) {
                return dom_element_get_attribute(ele,
                                doc->memoised[hds_frame_border], frame_border);
        }

        *frame_border = ele->frame_border_default;
        if (*frame_border != NULL)
                dom_string_ref(*frame_border);
        return DOM_NO_ERR;
}

/**
 * Get the frame_border property
 *
 * \param ele		The dom_html_iframe_element object
 * \param scrolling		The returned status
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_html_iframe_element_get_scrolling(
		dom_html_iframe_element *ele,
		dom_string **scrolling)
{
	dom_html_document *doc;
        bool has_value = false;
        dom_exception err;

        doc = (dom_html_document *) ((dom_node_internal *) ele)->owner;

        err = dom_element_has_attribute(ele,
                         doc->memoised[hds_scrolling], &has_value);
        if(err !=DOM_NO_ERR)
                return err;

        if(has_value) {
                return dom_element_get_attribute(ele,
                                doc->memoised[hds_scrolling], scrolling);
        }

        *scrolling = ele->scrolling_default;
        if (*scrolling != NULL)
                dom_string_ref(*scrolling);
        return DOM_NO_ERR;
}

/**
 * Get the frame_border property
 *
 * \param ele		The dom_html_iframe_element object
 * \param content_document		The returned status
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_html_iframe_element_get_content_document(
		dom_html_iframe_element *ele,
		dom_document **content_document)
{
	*content_document = dom_node_get_owner(ele);
	return DOM_NO_ERR;
}

