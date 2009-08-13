/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_events_event_target_h_
#define dom_events_event_target_h_

#include <stdbool.h>
#include <dom/core/exceptions.h>

struct dom_string;
struct dom_event_listener;
struct dom_event;
struct dom_node_internal;

typedef struct dom_node_internal dom_event_target;

dom_exception _dom_event_target_add_event_listener(dom_event_target *et,
		struct dom_string *type, struct dom_event_listener *listener, 
		bool capture);
#define dom_event_target_add_event_listener(et, t, l, c) \
		_dom_event_target_add_event_listener((dom_event_target *) (et),\
		(struct dom_string *) (t), (struct dom_event_listener *) (l), \
		(bool) (c))

dom_exception _dom_event_target_remove_event_listener(dom_event_target *et,
		struct dom_string *type, struct dom_event_listener *listener, 
		bool capture);
#define dom_event_target_remove_event_listener(et, t, l, c) \
		_dom_event_target_remove_event_listener(\
		(dom_event_target *) (et), (struct dom_string *) (t),\
		(struct dom_event_listener *) (l), (bool) (c))

dom_exception _dom_event_target_dispatch_event(dom_event_target *et,
		struct dom_event *evt, bool *success);
#define dom_event_target_dispatch_event(et, e, s) \
		_dom_event_target_dispatch_event((dom_event_target *) (et),\
		(struct dom_event *) (e), (bool *) (s))

dom_exception _dom_event_target_add_event_listener_ns(dom_event_target *et,
		struct dom_string *namespace, struct dom_string *type, 
		struct dom_event_listener *listener, bool capture);
#define dom_event_target_add_event_listener_ns(et, n, t, l, c) \
		_dom_event_target_add_event_listener_ns(\
		(dom_event_target *) (et), (struct dom_string *) (n),\
		(struct dom_string *) (t), (struct dom_event_listener *) (l),\
		(bool) (c))

dom_exception _dom_event_target_remove_event_listener_ns(dom_event_target *et,
		struct dom_string *namespace, struct dom_string *type, 
		struct dom_event_listener *listener, bool capture);
#define dom_event_target_remove_event_listener_ns(et, n, t, l, c) \
		_dom_event_target_remove_event_listener_ns(\
		(dom_event_target *) (et), (struct dom_string *) (n),\
		(struct dom_string *) (t), (struct dom_event_listener *) (l),\
		(bool) (c))

#endif

