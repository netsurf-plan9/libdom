/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <stddef.h>

#include <dom/bootstrap/implpriv.h>
#include <dom/bootstrap/implregistry.h>

#include <dom/core/impllist.h>
#include <dom/core/implementation.h>

void dom_implementation_list_destroy(struct dom_implementation_list *list);

/**
 * Item in list of registered DOM implementation sources
 */
struct dom_impl_src_item {
	struct dom_implementation_source *source;	/**< Source */

	struct dom_impl_src_item *next;		/**< Next in list */
	struct dom_impl_src_item *prev;		/**< Previous in list */
};

static struct dom_impl_src_item *sources; /**< List of registered sources */
static dom_alloc alloc;
static void *pw;

/**
 * Initialise the implementation registry
 *
 * \param allocator  The memory allocator
 * \param ptr        Private data pointer of allocator
 * \return DOM_NO_ERR on success
 */
dom_exception dom_implregistry_initialise(
		dom_alloc allocator, void *ptr)
{
	alloc = allocator;
	pw = ptr;

	return DOM_NO_ERR;
}

/**
 * Retrieve a DOM implementation from the registry
 *
 * \param features  String containing required features
 * \param impl      Pointer to location to receive implementation
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function. The implementation's
 * destroy() method will be called once it is no longer used.
 *
 * The implementation will be referenced, so the client need not
 * do this explicitly. The client must unref the implementation
 * once it has finished with it.
 */
dom_exception dom_implregistry_get_dom_implementation(
		struct dom_string *features,
		struct dom_implementation **impl)
{
	struct dom_impl_src_item *item;
	struct dom_implementation *found = NULL;
	dom_exception err;

	for (item = sources; item; item = item->next) {
		err = item->source->get_dom_implementation(features, &found);
		if (err != DOM_NO_ERR)
			return err;

		if (found != NULL)
			break;
	}

	*impl = found;

	return DOM_NO_ERR;
}

/**
 * Get a list of DOM implementations that support the requested
 * features
 *
 * \param features  String containing required features
 * \param list      Pointer to location to receive list
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * Any memory allocated by this call should be allocated using
 * the provided memory (de)allocation function.
 *
 * List nodes reference the implementation objects they point to.
 *
 * The list will be referenced, so the client need not do this
 * explicitly. The client must unref the list once it has finished
 * with it.
 */
dom_exception dom_implregistry_get_dom_implementation_list(
		struct dom_string *features,
		struct dom_implementation_list **list)
{
	struct dom_implementation_list *l;
	struct dom_impl_src_item *item;
	dom_exception err;

	l = alloc(NULL, sizeof(struct dom_implementation_list), pw);
	if (l == NULL)
		return DOM_NO_MEM_ERR;

	l->head = NULL;
	l->refcnt = 1;
	l->destroy = dom_implementation_list_destroy;

	for (item = sources; item; item = item->next) {
		struct dom_implementation_list *plist = NULL;
		struct dom_implementation_list_item *plast = NULL;

		err = item->source->get_dom_implementation_list(features,
				&plist);
		if (err != DOM_NO_ERR) {
			dom_implementation_list_unref(l);
			return err;
		}

		if (plist == NULL)
			continue;

		if (plist->head == NULL) {
			dom_implementation_list_unref(plist);
			continue;
		}

		/* Get last item in list for this source */
		for (plast = plist->head; plast; plast = plast->next) {
			if (plast->next == NULL)
				break;
		}

		/* Prepend list for this source onto result list */
		plast->next = l->head;
		if (l->head != NULL)
			l->head->prev = plast;
		l->head = plist->head;

		/* Invalidate entire content of list for this source */
		plist->head = NULL;

		/* And unref it */
		dom_implementation_list_unref(plist);
	}

	if (l->head == NULL) {
		*list = NULL;
		dom_implementation_list_unref(l);
	} else {
		*list = l;
	}

	return DOM_NO_ERR;
}

/**
 * Register a DOM implementation source with the DOM library
 *
 * \param source  The implementation source to register
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion.
 */
dom_exception dom_register_source(struct dom_implementation_source *source)
{
	struct dom_impl_src_item *item;

	item = alloc(NULL, sizeof(struct dom_impl_src_item), pw);
	if (item == NULL)
		return DOM_NO_MEM_ERR;

	item->source = source;

	item->next = sources;
	item->prev = NULL;

	if (sources != NULL)
		sources->prev = item;

	sources = item;

	return DOM_NO_ERR;
}

/**
 * Destroy a dom_implementation_list
 *
 * \param list	The list to destory
 */
void dom_implementation_list_destroy(struct dom_implementation_list *list)
{
	struct dom_implementation_list_item *i, *j;

	/* Destroy all list entries */
	for (i = list->head; i; i = j) {
		j = i->next;

		/* Unreference the implementation */
		dom_implementation_unref(i->impl);

		/* And free the entry */
		alloc(i, 0, pw);
	}

	/* Free the list object */
	alloc(list, 0, pw);
}
