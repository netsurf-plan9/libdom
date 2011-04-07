/* * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_events_mouse_wheel_event_h_
#define dom_events_mouse_wheel_event_h_

#include <stdbool.h>
#include <dom/core/exceptions.h>
#include <dom/core/string.h>
#include <dom/events/event_target.h>

struct dom_abstract_view;

typedef struct dom_mouse_wheel_event dom_mouse_wheel_event;

dom_exception _dom_mouse_wheel_event_get_wheel_delta(
		dom_mouse_wheel_event *evt, long *d);
#define dom_mouse_wheel_event_get_wheel_delta(e, d) \
		_dom_mouse_wheel_event_get_wheel_delta( \
		(dom_mouse_wheel_event *) (e), (long *) (d))

dom_exception _dom_mouse_wheel_event_init_ns(
		dom_mouse_wheel_event *evt, dom_string *namespace,
		dom_string *type,  bool bubble, bool cancelable,
		struct dom_abstract_view *view, long detail, long screen_x,
		long screen_y, long client_x, long client_y,
		unsigned short button, dom_event_target *et,
		dom_string *modifier_list, long wheel_delta);
#define dom_mouse_wheel_event_init_ns(e, n, t, b, c, v, \
		d, sx, sy, cx, cy, button, et, ml, dt) \
		_dom_mouse_wheel_event_init_ns((dom_mouse_wheel_event *) (e), \
		(dom_string *) (n), (dom_string *) (t), \
		(bool) (b), (bool) (c), (struct dom_abstract_view *) (v),\
		(long) (d), (long) (sx), (long) (sy), (long) (cx), (long) (cy),\
		(unsigned short) (button), (dom_event_target *) (et),\
		(dom_string *) (ml), (long) (dt))

#endif

