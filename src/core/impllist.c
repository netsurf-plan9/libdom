/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/bootstrap/implpriv.h>
#include <dom/core/implementation.h>
#include <dom/core/impllist.h>

extern void dom_implementation_list_destroy(
		struct dom_implementation_list *list);

/**
 * Claim a reference on a DOM implementation list
 *
 * \param list  The list to claim a reference on
 */
void dom_implementation_list_ref(struct dom_implementation_list *list)
{
	list->refcnt++;
}

/**
 * Release a reference from a DOM implementation list
 *
 * \param list  The list to release the reference from
 *
 * If the reference count reaches zero, any memory claimed by the
 * list will be released
 */
void dom_implementation_list_unref(struct dom_implementation_list *list)
{
	if (--list->refcnt == 0) {
		dom_implementation_list_destroy(list);
	}
}

/**
 * Retrieve the length of a DOM implementation list
 *
 * \param list    The list to retrieve the length of
 * \param length  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_implementation_list_get_length(
		struct dom_implementation_list *list, unsigned long *length)
{
	unsigned long count = 0;
	struct dom_implementation_list_item *i;

	for (i = list->head; i; i = i->next)
		count++;

	*length = count;

	return DOM_NO_ERR;
}

/**
 * Retrieve an item by index from a DOM implementation list
 *
 * \param list   The list to retrieve the item from
 * \param index  The list index to retrieve
 * \param impl   Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * ::index is a zero-based index into ::list.
 * ::index lies in the range [0, length-1]
 *
 * The returned implementation will have had its reference count increased.
 * The client should unref the implementation once it has finished with it.
 */
dom_exception dom_implementation_list_item(
		struct dom_implementation_list *list, unsigned long index,
		struct dom_implementation **impl)
{
	unsigned long idx = 0;
	struct dom_implementation_list_item *i;

	for (i = list->head; i; i = i->next) {
		if (idx == index)
			break;

		idx++;
	}

	if (i == NULL) {
		*impl = NULL;
	} else {
		dom_implementation_ref(i->impl);
		*impl = i->impl;
	}

	return DOM_NO_ERR;
}
