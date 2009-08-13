/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_events_internal_mouse_multi_wheel_event_h_
#define dom_events_internal_mouse_multi_wheel_event_h_

#include <dom/events/mouse_multi_wheel_event.h>

#include "events/mouse_event.h"

/**
 * The MouseMultiWheelEvent
 */
struct dom_mouse_multi_wheel_event {
	struct dom_mouse_event base;	/**< The base class */

	long x;	/**< The wheelDeltaX */
	long y;	/**< The wheelDeltaY */
	long z;	/**< The wheelDeltaZ */
};

/* Constructor */
dom_exception _dom_mouse_multi_wheel_event_create(struct dom_document *doc, 
		struct dom_mouse_multi_wheel_event **evt);

/* Destructor */
void _dom_mouse_multi_wheel_event_destroy(struct dom_document *doc, 
		struct dom_mouse_multi_wheel_event *evt);

/* Initialise function */
dom_exception _dom_mouse_multi_wheel_event_initialise(struct dom_document *doc,
		struct dom_mouse_multi_wheel_event *evt);

/* Finalise function */
#define _dom_mouse_multi_wheel_event_finalise _dom_mouse_event_finalise


#endif


