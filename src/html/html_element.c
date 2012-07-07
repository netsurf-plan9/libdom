/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku.com>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "html/html_document.h"
#include "html/html_element.h"

#include "core/node.h"
#include "core/attr.h"
#include "core/document.h"
#include "utils/utils.h"

struct dom_html_element_vtable _dom_html_element_vtable = {
	{
		{
			{
				DOM_NODE_EVENT_TARGET_VTABLE
			},
			DOM_NODE_VTABLE_ELEMENT,
		},
		DOM_ELEMENT_VTABLE
	},
	DOM_HTML_ELEMENT_VTABLE
};

static struct dom_element_protected_vtable _dom_html_element_protect_vtable = {
	{
		DOM_HTML_ELEMENT_PROTECT_VTABLE
	},
	DOM_ELEMENT_PROTECT_VTABLE
};

dom_exception _dom_html_element_create(struct dom_html_document *doc,
		dom_string *name, dom_string *namespace,
		dom_string *prefix, struct dom_html_element **result)
{
	dom_exception error;
	dom_html_element *el;

	el = malloc(sizeof(struct dom_html_element));
	if (el == NULL)
		return DOM_NO_MEM_ERR;

	el->base.base.base.vtable = &_dom_html_element_vtable;
	el->base.base.vtable = &_dom_html_element_protect_vtable;

	error = _dom_html_element_initialise(doc, el, name, namespace,
			prefix);
	if (error != DOM_NO_ERR) {
		free(el);
		return error;
	}

	*result = el;

	return DOM_NO_ERR;
}

dom_exception _dom_html_element_initialise(struct dom_html_document *doc,
		struct dom_html_element *el, dom_string *name, 
		dom_string *namespace, dom_string *prefix)
{
	dom_exception err;

	err = _dom_element_initialise(&doc->base, &el->base, name, namespace, prefix);
	if (err != DOM_NO_ERR)
		return err;
	
	return err;
}

void _dom_html_element_finalise(struct dom_html_element *ele)
{
	_dom_element_finalise(&ele->base);
}

/*------------------------------------------------------------------------*/
/* The protected virtual functions */

/* The virtual destroy function, see src/core/node.c for detail */
void _dom_html_element_destroy(dom_node_internal *node)
{
	dom_html_element *html = (dom_html_element *) node;

	_dom_html_element_finalise(html);

	free(html);
}

/* The virtual copy function, see src/core/node.c for detail */
dom_exception _dom_html_element_copy(dom_node_internal *old,
		dom_node_internal **copy)
{
	return _dom_element_copy(old, copy);
}

/*-----------------------------------------------------------------------*/
/* API functions */

#define SIMPLE_GET_SET(fattr,attr)                                    \
dom_exception _dom_html_element_get_##fattr(dom_html_element *element, \
					   dom_string **fattr)		\
{									\
	dom_exception ret;						\
	dom_string *_memo_##attr;					\
									\
	_memo_##attr =							\
		((struct dom_html_document *)				\
		 ((struct dom_node_internal *)element)->owner)->memoised[hds_##attr]; \
									\
	ret = dom_element_get_attribute(element, _memo_##attr, fattr);	\
									\
	return ret;							\
}									\
									\
dom_exception _dom_html_element_set_##fattr(dom_html_element *element,	\
					   dom_string *fattr)		\
{									\
	dom_exception ret;						\
	dom_string *_memo_##attr;					\
									\
	_memo_##attr =							\
		((struct dom_html_document *)				\
		 ((struct dom_node_internal *)element)->owner)->memoised[hds_##attr]; \
									\
	ret = dom_element_set_attribute(element, _memo_##attr, fattr);	\
									\
	return ret;							\
}

SIMPLE_GET_SET(id,id)
SIMPLE_GET_SET(title,title)
SIMPLE_GET_SET(lang,lang)
SIMPLE_GET_SET(dir,dir)
SIMPLE_GET_SET(class_name,class)

/*-----------------------------------------------------------------------*/
/* Common functions */

/**
 * Get the a bool property
 *
 * \param ele   The dom_html_element object
 * \param name  The name of the attribute
 * \param len   The length of ::name
 * \param has   The returned status
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_html_element_get_bool_property(dom_html_element *ele,
		const char *name, unsigned long len, bool *has)
{
	dom_string *str = NULL;
	dom_attr *a = NULL;
	dom_exception err;

	err = dom_string_create((const uint8_t *) name, len, &str);
	if (err != DOM_NO_ERR)
		goto fail;

	err = dom_element_get_attribute_node(ele, str, &a);
	if (err != DOM_NO_ERR)
		goto cleanup1;

	if (a != NULL) {
		*has = true;
	} else {
		*has = false;
	}

	dom_node_unref(a);

cleanup1:
	dom_string_unref(str);

fail:
	return err;
}

/**
 * Set a bool property
 *
 * \param ele   The dom_html_element object
 * \param name  The name of the attribute
 * \param len   The length of ::name
 * \param has   The status
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_html_element_set_bool_property(dom_html_element *ele,
		const char *name, unsigned long len, bool has)
{
	dom_string *str = NULL;
	dom_attr *a = NULL;
	dom_exception err;

	err = dom_string_create((const uint8_t *) name, len, &str);
	if (err != DOM_NO_ERR)
		goto fail;

	err = dom_element_get_attribute_node(ele, str, &a);
	if (err != DOM_NO_ERR)
		goto cleanup1;
	
	if (a != NULL && has == false) {
		dom_attr *res = NULL;

		err = dom_element_remove_attribute_node(ele, a, &res);
		if (err != DOM_NO_ERR)
			goto cleanup2;

		dom_node_unref(res);
	} else if (a == NULL && has == true) {
		dom_document *doc = dom_node_get_owner(ele);
		dom_attr *res = NULL;

		err = _dom_attr_create(doc, str, NULL, NULL, true, &a);
		if (err != DOM_NO_ERR) {
			goto cleanup1;
		}

		err = dom_element_set_attribute_node(ele, a, &res);
		if (err != DOM_NO_ERR)
			goto cleanup2;

		dom_node_unref(res);
	}

cleanup2:
	dom_node_unref(a);

cleanup1:
	dom_string_unref(str);

fail:
	return err;
}

static char *_strndup(const char *s, size_t n)
{
	size_t len;
	char *s2;

	for (len = 0; len != n && s[len] != '\0'; len++)
		continue;

	s2 = malloc(len + 1);
	if (s2 == NULL)
		return NULL;

	memcpy(s2, s, len);
	s2[len] = '\0';
	return s2;
}

/**
 * Get the a long property
 *
 * \param ele   The dom_html_element object
 * \param name  The name of the attribute
 * \param len   The length of ::name
 * \param value   The returned value
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_html_element_get_long_property(dom_html_element *ele,
		const char *name, unsigned long len, unsigned long *value)
{
	dom_string *str = NULL, *s2 = NULL;
	dom_attr *a = NULL;
	dom_exception err;

	err = dom_string_create((const uint8_t *) name, len, &str);
	if (err != DOM_NO_ERR)
		goto fail;

	err = dom_element_get_attribute_node(ele, str, &a);
	if (err != DOM_NO_ERR)
		goto cleanup1;

	if (a != NULL) {
		err = dom_node_get_text_content(a, &s2);
		if (err == DOM_NO_ERR) {
			char *s3 = _strndup(dom_string_data(s2),
					    dom_string_byte_length(s2));
			*value = strtoul(s3, NULL, 0);
			free(s3);
			dom_string_unref(s2);
		}
	} else {
		*value = 0;
	}

	dom_node_unref(a);

cleanup1:
	dom_string_unref(str);

fail:
	return err;
}

/**
 * Set a long property
 *
 * \param ele   The dom_html_element object
 * \param name  The name of the attribute
 * \param len   The length of ::name
 * \param value   The value
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception dom_html_element_set_long_property(dom_html_element *ele,
		const char *name, unsigned long len, unsigned long value)
{
	dom_string *str = NULL, *svalue = NULL;
	dom_exception err;
	char numbuffer[32];

	err = dom_string_create((const uint8_t *) name, len, &str);
	if (err != DOM_NO_ERR)
		goto fail;
	
	if (snprintf(numbuffer, 32, "%lu", value) == 32)
		numbuffer[31] = '\0';
	
	err = dom_string_create((const uint8_t *) numbuffer,
				strlen(numbuffer), &svalue);
	if (err != DOM_NO_ERR)
		goto cleanup;
	
	err = dom_element_set_attribute(ele, svalue, str);
	
	dom_string_unref(svalue);
cleanup:
	dom_string_unref(str);

fail:
	return err;
}
