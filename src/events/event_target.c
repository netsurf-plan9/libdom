/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>

#include "events/event.h"
#include "events/event_listener.h"
#include "events/event_target.h"

#include "core/document.h"
#include "core/node.h"
#include "core/string.h"

#include "utils/utils.h"
#include "utils/validate.h"

/* The number of chains in the hash table used for hash event types */
#define CHAINS 11

static uint32_t event_target_hash(void *key, void *pw)
{
	UNUSED(pw);

	return dom_string_hash(key);
}

static void event_target_destroy_key(void *key, void *pw)
{
	UNUSED(pw);

	dom_string_unref(key);
}

static void event_target_destroy_value(void *value, void *pw)
{
	struct listener_entry *le = NULL;
	struct list_entry *i = (struct list_entry *) value;

	UNUSED(pw);

	while (i != i->next) {
		le = (struct listener_entry *) i->next;
		list_del(i->next);
		dom_event_listener_unref(le->listener);
		free(le);
	}

	le = (struct listener_entry *) i;
	list_del(i);
	dom_event_listener_unref(le->listener);
	free(le);
}

static bool event_target_key_isequal(void *key1, void *key2, void *pw)
{
	UNUSED(pw);

	return dom_string_isequal(key1, key2);
}

static const dom_hash_vtable event_target_vtable = {
	event_target_hash,
	NULL,
	event_target_destroy_key,
	NULL,
	event_target_destroy_value,
	event_target_key_isequal
};

/* Initialise this EventTarget */
dom_exception _dom_event_target_internal_initialise(
		dom_event_target_internal *eti)
{
	eti->listeners = _dom_hash_create(CHAINS, &event_target_vtable, NULL);
	if (eti->listeners == NULL)
		return DOM_NO_MEM_ERR;

	eti->ns_listeners = NULL;

	return DOM_NO_ERR;
}

/* Finalise this EventTarget */
void _dom_event_target_internal_finalise(dom_event_target_internal *eti)
{
	_dom_hash_destroy(eti->listeners);

	/* TODO: Now, we did not support the EventListener with namespace,
	 * when we support it, we should deal with the ns_listeners hash 
	 * table, too.
	 */
}

/*-------------------------------------------------------------------------*/
/* The public API */

/**
 * Add an EventListener to the EventTarget
 *
 * \param et        The EventTarget object
 * \param type      The event type which this event listener listens for
 * \param listener  The event listener object
 * \param capture   Whether add this listener in the capturing phase
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_event_target_add_event_listener(
		dom_event_target_internal *eti,
		dom_string *type, struct dom_event_listener *listener, 
		bool capture)
{
	struct listener_entry *le = NULL;
	dom_string *t = NULL;

	le = malloc(sizeof(struct listener_entry));
	if (le == NULL)
		return DOM_NO_MEM_ERR;
	
	/* Initialise the listener_entry */
	list_init(&le->list);
	le->listener = listener;
	dom_event_listener_ref(listener);
	le->capture = capture;

	t = dom_string_ref(type);

	/* Find the type of this event */
	struct list_entry *item = (struct list_entry *) _dom_hash_get(
			eti->listeners, t);
	if (item == NULL) {
		/* If there is no item in the hash table, we should add the 
		 * first */
		_dom_hash_add(eti->listeners, t, &le->list, false);
	} else {
		/* Append this listener to the end of the list */
		list_append(item, &le->list);
	}

	return DOM_NO_ERR;
}

/**
 * Remove an EventListener from the EventTarget
 *
 * \param et        The EventTarget object
 * \param type      The event type this listener is registered for 
 * \param listener  The listener object
 * \param capture   Whether the listener is registered at the capturing phase
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_event_target_remove_event_listener(
		dom_event_target_internal *eti,
		dom_string *type, struct dom_event_listener *listener, 
		bool capture)
{
	struct listener_entry *le = NULL;

	/* Find the type of this event */
	struct list_entry *item = (struct list_entry *) _dom_hash_get(
			eti->listeners, type);
	if (item == NULL) {
		/* There is no such event listener */
		return DOM_NO_ERR;
	} else {
		struct list_entry *i = item;
		do {
			le = (struct listener_entry *) i;
			if (le->listener == listener && 
					le->capture == capture) {
				/* We found the listener */
				list_del(i);
				dom_event_listener_unref(le->listener);
				free(le);
				break;
			}
			i = i->next;
		} while(i != item);
	}

	return DOM_NO_ERR;
}

/**
 * Add an EventListener
 *
 * \param et         The EventTarget object
 * \param namespace  The namespace of this listener
 * \param type       The event type which this event listener listens for
 * \param listener   The event listener object
 * \param capture    Whether add this listener in the capturing phase
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 *
 * We don't support this API now, so it always return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_event_target_add_event_listener_ns(
		dom_event_target_internal *eti,
		dom_string *namespace, dom_string *type, 
		struct dom_event_listener *listener, bool capture)
{
	UNUSED(eti);
	UNUSED(namespace);
	UNUSED(type);
	UNUSED(listener);
	UNUSED(capture);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Remove an EventListener
 *
 * \param et         The EventTarget object
 * \param namespace  The namespace of this listener
 * \param type       The event type which this event listener listens for
 * \param listener   The event listener object
 * \param capture    Whether add this listener in the capturing phase
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 *
 * We don't support this API now, so it always return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_event_target_remove_event_listener_ns(
		dom_event_target_internal *eti,
		dom_string *namespace, dom_string *type, 
		struct dom_event_listener *listener, bool capture)
{
	UNUSED(eti);
	UNUSED(namespace);
	UNUSED(type);
	UNUSED(listener);
	UNUSED(capture);

	return DOM_NOT_SUPPORTED_ERR;
}

/*-------------------------------------------------------------------------*/

/**
 * Dispatch an event on certain EventTarget
 *
 * \param et       The EventTarget object
 * \param eti      Internal EventTarget object
 * \param evt      The event object
 * \param success  Indicates whether any of the listeners which handled the 
 *                 event called Event.preventDefault(). If 
 *                 Event.preventDefault() was called the returned value is 
 *                 false, else it is true.
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_event_target_dispatch(dom_event_target *et,
		dom_event_target_internal *eti, 
		struct dom_event *evt, dom_event_flow_phase phase,
		bool *success)
{
	dom_string *t = evt->type; 

	struct list_entry *item = (struct list_entry *) _dom_hash_get(
			eti->listeners, t);
	if (item == NULL) {
		/* There is no such event listener */
		return DOM_NO_ERR;
	} else {
		/* Call the handler for each listener */
		struct list_entry *i = item;
		/* Fill the Event fields */
		evt->current = et;
		do {
			struct listener_entry *le = 
					(struct listener_entry *) i;
			assert(le->listener->handler != NULL);
			if ((le->capture == true && 
				phase == DOM_CAPTURING_PHASE) ||
					(le->capture == false && 
						phase == DOM_BUBBLING_PHASE) ||
					(evt->target == evt->current && 
						phase == DOM_AT_TARGET)) {
				/* We found the listener */
				le->listener->handler(evt, le->listener->pw);
				/* If the handler call 
				 * stopImmediatedPropagation, we should
				 * break */
				if (evt->stop_now == true)
					break;
			}
			i = i->next;
		} while(i != item);
	}

	if (evt->prevent_default == true)
		*success = false;

	return DOM_NO_ERR;
}

