/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_events_document_event_h_
#define dom_events_document_event_h_

#include <dom/core/exceptions.h>

struct dom_string;
struct dom_event;
struct dom_document;
struct lwc_context_s;
struct lwc_string_s;

typedef struct dom_document dom_document_event;

/**
 * The default action fetcher
 *
 * @note: When the implementation reach the end of the event flow, it will call
 * this function to get the default action handler. If it does not return a
 * NULL, the returned dom_event_listener will be invoked as the event is not
 * canceled.
 */
typedef struct dom_event_listener *(*dom_events_default_action_fetcher)
		(struct lwc_string_s *name, struct lwc_string_s *type);

dom_exception _dom_document_event_create_event(dom_document_event *de,
		struct dom_string *type, struct dom_event **evt);
#define dom_document_event_create_event(d, t, e) \
		_dom_document_event_create_event((dom_document_event *) (d), \
		(struct dom_string *) (t), (struct dom_event **) (e))

dom_exception _dom_document_event_can_dispatch(dom_document_event *de,
		struct dom_string *namespace, struct dom_string *type,
		bool* can);
#define dom_document_event_can_dispatch(d, n, t, c) \
		_dom_document_event_can_dispatch((dom_document_event *) (d), \
		(struct dom_string *) (n), (struct dom_string *) (t),\
		(bool *) (c))

#endif

