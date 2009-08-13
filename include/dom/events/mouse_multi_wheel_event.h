/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_events_mouse_multi_wheel_event_h_
#define dom_events_mouse_multi_wheel_event_h_

#include <stdbool.h>
#include <dom/core/exceptions.h>
#include <dom/events/event_target.h>

struct dom_string;
struct dom_abstract_view;

typedef struct dom_mouse_multi_wheel_event dom_mouse_multi_wheel_event;

dom_exception _dom_mouse_multi_wheel_event_get_wheel_delta_x(
		dom_mouse_multi_wheel_event *evt, long *x);
#define dom_mouse_multi_wheel_event_get_wheel_delta_x(e, x) \
		_dom_mouse_multi_wheel_event_get_wheel_delta_x( \
		(dom_mouse_multi_wheel_event *) (e), (long *) (x))

dom_exception _dom_mouse_multi_wheel_event_get_wheel_delta_y(
		dom_mouse_multi_wheel_event *evt, long *y);
#define dom_mouse_multi_wheel_event_get_wheel_delta_y(e, y) \
		_dom_mouse_multi_wheel_event_get_wheel_delta_y( \
		(dom_mouse_multi_wheel_event *) (e), (long *) (y))

dom_exception _dom_mouse_multi_wheel_event_get_wheel_delta_z(
		dom_mouse_multi_wheel_event *evt, long *z);
#define dom_mouse_multi_wheel_event_get_wheel_delta_z(e, z) \
		_dom_mouse_multi_wheel_event_get_wheel_delta_z( \
		(dom_mouse_multi_wheel_event *) (e), (long *) (z))

dom_exception _dom_mouse_multi_wheel_event_init_ns(
		dom_mouse_multi_wheel_event *evt, struct dom_string *namespace,
		struct dom_string *type,  bool bubble, bool cancelable,
		struct dom_abstract_view *view, long detail, long screen_x,
		long screen_y, long client_x, long client_y,
		unsigned short button, dom_event_target *et,
		struct dom_string *modifier_list, long wheel_delta_x,
		long wheel_delta_y, long wheel_delta_z);
#define dom_mouse_multi_wheel_event_init_ns(e, n, t, b, c, v, \
		d, sx, sy, cx, cy, button, et, ml, x, y, z) \
		_dom_mouse_multi_wheel_event_init_ns( \
		(dom_mouse_multi_wheel_event *) (e), (struct dom_string *) (n),\
		(struct dom_string *) (t), (bool) (b), (bool) (c), \
		(struct dom_abstract_view *) (v), (long) (d), (long) (sx), \
		(long) (sy), (long) (cx), (long) (cy),\
		(unsigned short) (button), (dom_event_target *) (et),\
		(struct dom_string *) (ml), (long) (x), (long) (y), (long) (z))

#endif
