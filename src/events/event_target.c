/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>

#include <libwapcaplet/libwapcaplet.h>

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

/* Entry for a EventTarget, used to record the bubbling list */
typedef struct dom_event_target_entry {
	struct list_entry entry;	/**< The list entry */
	dom_event_target *et;	/**< The node */
} dom_event_target_entry;

/* Hash key/value functions */
static void *_key(void *key, void *key_pw, dom_alloc alloc, void *pw, 
		bool clone);
static void *_value(void *value, void *value_pw, dom_alloc alloc,
		void *pw, bool clone);


/* Initialise this EventTarget */
dom_exception _dom_event_target_internal_initialise(struct dom_document *doc,
		dom_event_target_internal *eti)
{
	UNUSED(doc);
	eti->listeners = NULL;
	eti->ns_listeners = NULL;

	return DOM_NO_ERR;
}

/* Finalise this EventTarget */
void _dom_event_target_internal_finalise(struct dom_document *doc, 
		dom_event_target_internal *eti)
{
	if (eti->listeners != NULL)
		_dom_hash_destroy(eti->listeners, _key, NULL, _value, doc);
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
dom_exception _dom_event_target_add_event_listener(dom_event_target *et,
		struct dom_string *type, struct dom_event_listener *listener, 
		bool capture)
{
	struct listener_entry *le = NULL;
	struct dom_document *doc = dom_node_get_owner(et);
	assert(doc != NULL);

	struct dom_event_target_internal *eti = &et->eti;
	lwc_string *t = NULL;
	dom_exception err;

	/* If there is no hash table, we should create one firstly */
	if (eti->listeners == NULL) {
		err = _dom_document_create_hashtable(doc, CHAINS, 
				_dom_hash_hash_lwcstring, &eti->listeners);
		if (err != DOM_NO_ERR)
			return err;
	}

	err = dom_string_get_intern(type, &t);
	if (err != DOM_NO_ERR)
		return err;

	if (t == NULL) {
		err = _dom_string_intern(type, &t);
		if (err != DOM_NO_ERR)
			return err;
	} else {
		lwc_string_ref(t);
	}

	assert(t != NULL);

	le = (struct listener_entry *) _dom_document_alloc(doc, NULL, 
			sizeof(struct listener_entry));
	if (le == NULL)
		return DOM_NO_MEM_ERR;
	
	/* Initialise the listener_entry */
	list_init(&le->list);
	le->listener = listener;
	dom_event_listener_ref(listener);
	le->capture = capture;

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
dom_exception _dom_event_target_remove_event_listener(dom_event_target *et,
		struct dom_string *type, struct dom_event_listener *listener, 
		bool capture)
{
	struct listener_entry *le = NULL;
	struct dom_document *doc = dom_node_get_owner(et);
	if (doc == NULL) {
		/* TODO: In the progress of parsing, many Nodes in the DTD
		 * has no document at all, do nothing for this kind of node */
		return DOM_NO_ERR;
	}

	struct dom_event_target_internal *eti = &et->eti;
	lwc_string *t = NULL;
	dom_exception err;

	err = dom_string_get_intern(type, &t);
	if (err != DOM_NO_ERR)
		return err;

	if (t == NULL) {
		err = _dom_string_intern(type, &t);
		if (err != DOM_NO_ERR)
			return err;
	} else {
		lwc_string_ref(t);
	}

	assert(t != NULL);

	/* Find the type of this event */
	struct list_entry *item = (struct list_entry *) _dom_hash_get(
			eti->listeners, t);
	if (item == NULL) {
		/* There is no such event listener */
		lwc_string_unref(t);
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
				_dom_document_alloc(doc, le,
						sizeof(struct listener_entry));
				break;
			}
			i = i->next;
		} while(i != item);
	}

	return DOM_NO_ERR;
}

/**
 * Dispatch an event into the implementation's event model
 *
 * \param et       The EventTarget object
 * \param evt      The event object
 * \param success  Indicates whether any of the listeners which handled the 
 *                 event called Event.preventDefault(). If 
 *                 Event.preventDefault() was called the returned value is 
 *                 false, else it is true.
 * \return DOM_NO_ERR                     on success
 *         DOM_DISPATCH_REQUEST_ERR       If the event is already in dispatch
 *         DOM_UNSPECIFIED_EVENT_TYPE_ERR If the type of the event is Null or
 *                                        empty string.
 *         DOM_NOT_SUPPORTED_ERR          If the event is not created by 
 *                                        Document.createEvent
 *         DOM_INVALID_CHARACTER_ERR      If the type of this event is not a
 *                                        valid NCName.
 */
dom_exception _dom_event_target_dispatch_event(dom_event_target *et,
		struct dom_event *evt, bool *success)
{
	assert(et != NULL);
	assert(evt != NULL);

	dom_exception err, ret = DOM_NO_ERR;

	/* To test whether this event is in dispatch */
	if (evt->in_dispatch == true) {
		return DOM_DISPATCH_REQUEST_ERR;
	} else {
		evt->in_dispatch = true;
	}

	if (evt->type == NULL || lwc_string_length(evt->type) == 0) {
		return DOM_UNSPECIFIED_EVENT_TYPE_ERR;
	}

	if (evt->doc == NULL)
		return DOM_NOT_SUPPORTED_ERR;
	
	struct dom_document *doc = dom_node_get_owner(et);
	if (doc == NULL) {
		/* TODO: In the progress of parsing, many Nodes in the DTD has
		 * no document at all, do nothing for this kind of node */
		return DOM_NO_ERR;
	}

	struct dom_string *type = NULL;
	err = _dom_document_create_string_from_lwcstring(doc, evt->type, &type);
	if (err != DOM_NO_ERR)
		return err;
	
	if (_dom_validate_ncname(type) == false) {
		dom_string_unref(type);
		return DOM_INVALID_CHARACTER_ERR;
	}
	dom_string_unref(type);

	lwc_string *t = evt->type;
	dom_event_target_entry list;
	dom_event_target *target = et;

	assert(t != NULL);

	*success = true;

	/* Compose the event target list */
	list_init(&list.entry);
	list.et = et;
	dom_node_ref(et);
	target = target->parent;

	while (target != NULL) {
		dom_event_target_entry *l = (dom_event_target_entry *) 
				_dom_document_alloc(doc, NULL,
				sizeof(dom_event_target_entry));
		if (l == NULL) {
			ret = DOM_NO_MEM_ERR;
			goto cleanup;
		}
		list_append(&list.entry, &l->entry);
		l->et = target;
		dom_node_ref(target);
		target = target->parent;
	}

	/* Fill the target of the event */
	evt->target = et;
	evt->phase = DOM_CAPTURING_PHASE;

	/* The capture phase */
	struct list_entry *e = list.entry.prev;
	for (; e != &list.entry; e = e->prev) {
		dom_event_target_entry *l = (dom_event_target_entry *) e;
		err = _dom_event_target_dispatch(l->et, evt,
				DOM_CAPTURING_PHASE, success);
		if (err != DOM_NO_ERR) {
			ret = err;
			goto cleanup;
		}
		/* If the stopImmediatePropagation or stopPropagation is
		 * called, we should break */
		if (evt->stop_now == true || evt->stop == true)
			goto cleanup;
	}

	/* Target phase */
	evt->phase = DOM_AT_TARGET;
	evt->current = et;
	err = _dom_event_target_dispatch(et, evt, DOM_AT_TARGET, 
			success);
	if (evt->stop_now == true || evt->stop == true)
		goto cleanup;

	/* Bubbling phase */
	evt->phase = DOM_BUBBLING_PHASE;

	e = list.entry.next;
	for (; e != &list.entry; e = e->next) {
		dom_event_target_entry *l = (dom_event_target_entry *) e;
		err = _dom_event_target_dispatch(l->et, evt,
				DOM_BUBBLING_PHASE, success);
		if (err != DOM_NO_ERR) {
			ret = err;
			goto cleanup;
		}
		/* If the stopImmediatePropagation or stopPropagation is
		 * called, we should break */
		if (evt->stop_now == true || evt->stop == true)
			goto cleanup;
	}

	struct dom_document_event_internal *dei = &doc->dei;
	if (dei->actions == NULL || evt->prevent_default == true)
		goto cleanup;

	/* The default action */
	struct dom_string *nodename;
	err = dom_node_get_node_name(et, &nodename);
	if (err != DOM_NO_ERR) {
		ret = err;
		goto cleanup;
	}
	lwc_string *lnodename = NULL;
	err = dom_string_get_intern(nodename, &lnodename);
	if (err != DOM_NO_ERR) {
		dom_string_unref(nodename);
		ret = err;
		goto cleanup;
	}

	dom_event_listener *da = dei->actions(lnodename, t);
	if (da != NULL) {
		da->handler(evt, da->pw);
	}

	dom_string_unref(nodename);
	lwc_string_unref(lnodename);

cleanup:
	if (evt->prevent_default == true) {
		*success = false;
	}

	while (list.entry.next != &list.entry) {
		dom_event_target_entry *e = (dom_event_target_entry *)
				list.entry.next;
		dom_node_unref(e->et);
		list_del(list.entry.next);
		_dom_document_alloc(doc, e, 0);
	}

	dom_node_unref(et);

	return ret;
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
dom_exception _dom_event_target_add_event_listener_ns(dom_event_target *et,
		struct dom_string *namespace, struct dom_string *type, 
		struct dom_event_listener *listener, bool capture)
{
	UNUSED(et);
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
dom_exception _dom_event_target_remove_event_listener_ns(dom_event_target *et,
		struct dom_string *namespace, struct dom_string *type, 
		struct dom_event_listener *listener, bool capture)
{
	UNUSED(et);
	UNUSED(namespace);
	UNUSED(type);
	UNUSED(listener);
	UNUSED(capture);

	return DOM_NOT_SUPPORTED_ERR;
}

/*-------------------------------------------------------------------------*/

/* The key process function of the hash table, see utils/hash_table.h for
 * detail */
static void *_key(void *key, void *key_pw, dom_alloc alloc, void *pw, 
		bool clone)
{
	UNUSED(key_pw);
	UNUSED(alloc);
	UNUSED(pw);
	/* There should never be the requirement of clone the event listener
	 * list */
	assert(clone == false);
	UNUSED(clone);

	lwc_string_unref((lwc_string *) key);

	return NULL;
}

/* The value process function of the hash table, see utils/hash_table.h for
 * detail */
static void *_value(void *value, void *value_pw, dom_alloc alloc,
		void *pw, bool clone)
{
	UNUSED(alloc);
	UNUSED(pw);
	/* There should never be the requirement of clone the event listener
	 * list */
	assert(clone == false);
	UNUSED(clone);

	struct listener_entry *le = NULL;
	struct dom_document *doc = (struct dom_document *) value_pw;
	struct list_entry *i = (struct list_entry *) value;

	while(i != i->next) {
		le = (struct listener_entry *) i->next;
		list_del(i->next);
		dom_event_listener_unref(le->listener);
		_dom_document_alloc(doc, le, sizeof(struct listener_entry));
	}

	le = (struct listener_entry *) i;
	list_del(i);
	dom_event_listener_unref(le->listener);
	_dom_document_alloc(doc, le, sizeof(struct listener_entry));

	return NULL;
}

/*-------------------------------------------------------------------------*/

/**
 * Dispatch an event on certain EventTarget
 *
 * \param et       The EventTarget object
 * \param evt      The event object
 * \param success  Indicates whether any of the listeners which handled the 
 *                 event called Event.preventDefault(). If 
 *                 Event.preventDefault() was called the returned value is 
 *                 false, else it is true.
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_event_target_dispatch(dom_event_target *et, 
		struct dom_event *evt, dom_event_flow_phase phase,
		bool *success)
{
	struct dom_event_target_internal *eti = &et->eti;
	lwc_string *t = evt->type; 

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

/**
 * Dispatch a DOMNodeInserted/DOMNodeRemoved event
 *
 * \param doc      The document object
 * \param et       The EventTarget object
 * \param type     "DOMNodeInserted" or "DOMNodeRemoved"
 * \param related  The parent of the removed/inserted node
 * \param success  Whether this event's default action get called
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_dispatch_node_change_event(struct dom_document *doc,
		dom_event_target *et, dom_event_target *related, 
		dom_mutation_type change, bool *success)
{
	struct dom_mutation_event *evt;
	dom_exception err;

	err = _dom_mutation_event_create(doc, &evt);
	if (err != DOM_NO_ERR)
		return err;
	
	lwc_string *type = NULL;
	if (change == DOM_MUTATION_ADDITION) {
		err = _dom_document_create_lwcstring(doc, 
				(const uint8_t *) "DOMNodeInserted",
				SLEN("DOMNodeInserted"), &type);
		if (err != DOM_NO_ERR)
			goto cleanup;
	} else if (change == DOM_MUTATION_REMOVAL) {
		err = _dom_document_create_lwcstring(doc, 
				(const uint8_t *) "DOMNodeRemoval",
				SLEN("DOMNodeRemoved"), &type);
		if (err != DOM_NO_ERR)
			goto cleanup;
	} else {
		assert("Should never be here" == NULL);
	}

	dom_string *t = NULL;
	err = _dom_document_create_string_from_lwcstring(doc, type, &t);
	_dom_document_unref_lwcstring(doc, type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	/* Initiliase the event with corresponding parameters */
	err = dom_mutation_event_init(evt, t, true, false, related, NULL, NULL, 
			NULL, change);
	dom_string_unref(t);
	if (err != DOM_NO_ERR) {
		goto cleanup;
	}

	err = dom_event_target_dispatch_event(et, evt, success);
	if (err != DOM_NO_ERR)
		goto cleanup;
	
	/* Finalise the evt, and reuse it */
	_dom_mutation_event_finalise(doc, evt);
	/* Dispatch the DOMNodeInsertedIntoDocument/DOMNodeRemovedFromDocument 
	 * event */
	if (change == DOM_MUTATION_ADDITION) {
		err = _dom_document_create_lwcstring(doc, 
				(const uint8_t *) 
					"DOMNodeInsertedIntoDocument", 
				SLEN("DOMNodeInsertedIntoDocument"), &type);
		if (err != DOM_NO_ERR)
			goto cleanup;
	} else if (change == DOM_MUTATION_REMOVAL) {
		err = _dom_document_create_lwcstring(doc, 
				(const uint8_t *) "DOMNodeRemovedFromDocument", 
				SLEN("DOMNodeRemovedFromDocument"), &type);
		if (err != DOM_NO_ERR)
			goto cleanup;
	} else {
		assert("Should never be here" == NULL);
	}

	err = _dom_document_create_string_from_lwcstring(doc, type, &t);
	_dom_document_unref_lwcstring(doc, type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	/* Dispatch the events for its children */
	dom_event_target *target = et->first_child;
	while (target != NULL) {
		err = dom_mutation_event_init(evt, t, true, false, NULL,
				NULL, NULL, NULL, change);
		if (err != DOM_NO_ERR)
			goto cleanup;

		err = dom_event_target_dispatch_event(target, evt, success);
		if (err != DOM_NO_ERR)
			goto cleanup;

		dom_event_target *p = dom_node_get_parent(target);
		if (target->first_child != NULL) {
			target = target->first_child;
		} else if (target->next != NULL) {
			target = target->next;
		} else {
			while (p != et && target == p->last_child) {
				target = p;
				p = dom_node_get_parent(p);
			}

			target = target->next;
		}
		/* Finalise the event for reuse in next iteration */
		_dom_mutation_event_finalise(doc, evt);
	}

cleanup:
	_dom_mutation_event_destroy(doc, evt);

	return err;
}

/**
 * Dispatch a DOMAttrModified event
 *
 * \param doc        The Document object
 * \param et         The EventTarget
 * \param prev       The previous value before change
 * \param new        The new value after change
 * \param related    The related EventTarget
 * \param attr_name  The Attribute name
 * \param change     How this attribute change
 * \param success    Whether this event's default handler get called
 * \return DOM_NO_ERR on success, appropirate dom_exception on failure.
 */
dom_exception _dom_dispatch_attr_modified_event(struct dom_document *doc,
		dom_event_target *et, dom_string *prev, dom_string *new,
		dom_event_target *related, dom_string *attr_name, 
		dom_mutation_type change, bool *success)
{
	struct dom_mutation_event *evt;
	dom_exception err;

	err = _dom_mutation_event_create(doc, &evt);
	if (err != DOM_NO_ERR)
		return err;
	
	lwc_string *type = NULL;
	err = _dom_document_create_lwcstring(doc, 
			(const uint8_t *) "DOMAttrModified",
			SLEN("DOMAttrModified"), &type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	dom_string *t = NULL;
	err = _dom_document_create_string_from_lwcstring(doc, type, &t);
	_dom_document_unref_lwcstring(doc, type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	/* Initiliase the event with corresponding parameters */
	err = dom_mutation_event_init(evt, t, true, false, related, prev, new,
			attr_name, change);
	dom_string_unref(t);
	if (err != DOM_NO_ERR) {
		goto cleanup;
	}

	err = dom_event_target_dispatch_event(et, evt, success);

cleanup:
	_dom_mutation_event_destroy(doc, evt);

	return err;
}

/**
 * Dispatch a DOMCharacterDataModified event
 *
 * \param et    The EventTarget object
 * \param prev  The preValue of the DOMCharacterData
 * \param new   The newValue of the DOMCharacterData
 * \return DOM_NO_ERR on success, appropirate dom_exception on failure.
 *
 * TODO:
 * The character_data object may be a part of a Attr node, if so, another 
 * DOMAttrModified event should be dispatched, too. But for now, we did not
 * support any XML feature, so just leave it as this.
 */
dom_exception _dom_dispatch_characterdata_modified_event(
		struct dom_document *doc, dom_event_target *et,
		dom_string *prev, dom_string *new, bool *success)
{
	struct dom_mutation_event *evt;
	dom_exception err;

	err = _dom_mutation_event_create(doc, &evt);
	if (err != DOM_NO_ERR)
		return err;
	
	lwc_string *type = NULL;
	err = _dom_document_create_lwcstring(doc, 
			(const uint8_t *) "DOMCharacterDataModified",
			SLEN("DOMCharacterDataModified"), &type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	dom_string *t = NULL;
	err = _dom_document_create_string_from_lwcstring(doc, type, &t);
	_dom_document_unref_lwcstring(doc, type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	err = dom_mutation_event_init(evt, t, true, false, et, prev, new, NULL,
			DOM_MUTATION_MODIFICATION);
	dom_string_unref(t);
	if (err != DOM_NO_ERR) {
		goto cleanup;
	}

	err = dom_event_target_dispatch_event(et, evt, success);

cleanup:
	_dom_mutation_event_destroy(doc, evt);

	return err;
}

/**
 * Dispatch a DOMSubtreeModified event
 *
 * \param doc      The Document
 * \param et       The EventTarget object
 * \param success  The newValue of the DOMCharacterData
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_dispatch_subtree_modified_event(struct dom_document *doc,
		dom_event_target *et, bool *success)
{
	struct dom_mutation_event *evt;
	dom_exception err;

	err = _dom_mutation_event_create(doc, &evt);
	if (err != DOM_NO_ERR)
		return err;
	
	lwc_string *type = NULL;
	err = _dom_document_create_lwcstring(doc, 
			(const uint8_t *) "DOMSubtreeModified",
			SLEN("DOMSubtreeModified"), &type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	dom_string *t = NULL;
	err = _dom_document_create_string_from_lwcstring(doc, type, &t);
	_dom_document_unref_lwcstring(doc, type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	err = dom_mutation_event_init(evt, t, true, false, et, NULL, NULL, NULL,
			DOM_MUTATION_MODIFICATION);
	dom_string_unref(t);
	if (err != DOM_NO_ERR) {
		goto cleanup;
	}

	err = dom_event_target_dispatch_event(et, evt, success);

cleanup:
	_dom_mutation_event_destroy(doc, evt);

	return err;
}

