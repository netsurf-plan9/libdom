/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_internal_events_event_target_h_
#define dom_internal_events_event_target_h_

#include <dom/events/event.h>
#include <dom/events/mutation_event.h>
#include <dom/events/event_target.h>
#include <dom/events/event_listener.h>

#include "utils/list.h"
#include "utils/hashtable.h"

struct dom_document;
struct dom_string;

/**
 * Listener Entry
 */
struct listener_entry {
	struct list_entry list;	
		/**< The listener list registered at the same
		 * EventTarget */
	dom_event_listener *listener;	/**< The EventListener */
	bool capture;	/**< Whether this listener is in capture phase */
};

/**
 * EventTarget internal class
 */
struct dom_event_target_internal {
	struct dom_hash_table *listeners;	
			/**< The listeners of this EventTarget.
			 * The listeners are organized by the 
			 * type of events they listen for
			 */
	struct dom_hash_table *ns_listeners;
			/**< The listeners with namespace of thie EventTarget */
};

typedef struct dom_event_target_internal dom_event_target_internal;

/**
 * Constructor and destructor: Since this object is not intended to be 
 * allocated alone, it should be embedded into the Node object, there is
 * no constructor and destructor for it.
 */

/* Initialise this EventTarget */
dom_exception _dom_event_target_internal_initialise(struct dom_document *doc,
		dom_event_target_internal *eti);

/* Finalise this EventTarget */
void _dom_event_target_internal_finalise(struct dom_document *doc, 
		dom_event_target_internal *eti);

/* Dispatch the event on this node */
dom_exception _dom_event_target_dispatch(dom_event_target *et, 
		struct dom_event *evt, dom_event_flow_phase phase,
		bool *success);

/* Dispatch a DOMNodeInserted/DOMNodeRemoved event */
dom_exception _dom_dispatch_node_change_event(struct dom_document *doc,
		dom_event_target *et, dom_event_target *related, 
		dom_mutation_type change, bool *success);

/* Dispatch a DOMCharacterDataModified event */
dom_exception _dom_dispatch_characterdata_modified_event(
		struct dom_document *doc, dom_event_target *et,
		struct dom_string *prev, struct dom_string *new, bool *success);

/* Dispatch a DOMAttrModified event */
dom_exception _dom_dispatch_attr_modified_event(struct dom_document *doc,
		dom_event_target *et, struct dom_string *prev,
		struct dom_string *new, dom_event_target *related,
		struct dom_string *attr_name, dom_mutation_type change,
		bool *success);

/* Dispatch a DOMSubtreeModified event */
dom_exception _dom_dispatch_subtree_modified_event(struct dom_document *doc,
		dom_event_target *et, bool *success);

/* Dispatch a generic event */
dom_exception _dom_dispatch_generic_event(struct dom_document *doc,
		dom_event_target *et, const uint8_t *name, size_t len,
		bool bubble, bool cancelable, bool *success);
#endif
