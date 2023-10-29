#include "util.h"
#include "window.h"
#include "wm.h"
#include "workspace.h"

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>

static void map_request(xcb_generic_event_t *gevent)
{
	xcb_map_request_event_t *e = (xcb_map_request_event_t *)gevent;
	map_ws(current_s(&workspace), e->window);
	xcb_flush(conn);
	log_message("MAP_REQUEST:\t%d\n", current_s(&workspace)->subsidiaries.size);
}

static void destroy_notify(xcb_generic_event_t *gevent)
{
	xcb_destroy_notify_event_t *e = (xcb_destroy_notify_event_t *)gevent;
	unmap_ws(current_s(&workspace), e->window);
	xcb_flush(conn);
}

static void unmap_notify(xcb_generic_event_t *gevent)
{
	xcb_unmap_notify_event_t *e = (xcb_unmap_notify_event_t *)gevent;
	if (!window_exists(e->window)) {
		return;
	}
	unmap_ws(current_s(&workspace), e->window);
	xcb_flush(conn);
}

static void enter_notify(xcb_generic_event_t *gevent)
{
	xcb_enter_notify_event_t *e = (xcb_enter_notify_event_t *)gevent;
	if (e->mode == XCB_NOTIFY_MODE_NORMAL) {
		xcb_set_input_focus(conn, XCB_INPUT_FOCUS_PARENT, e->event, XCB_CURRENT_TIME);
	}
	xcb_flush(conn);
}

static void key_press(xcb_generic_event_t *gevent)
{
	xcb_key_press_event_t *e = (xcb_key_press_event_t *)gevent;
	log_message("KEY_PRESS: detail: %u, state: %u", e->detail, e->state);
}

void handle_event(xcb_generic_event_t *event)
{
	while ((event = xcb_wait_for_event(conn))) {
		switch (event->response_type & ~0x80) {
		case XCB_MAP_REQUEST:
			map_request(event);
			break;
		case XCB_DESTROY_NOTIFY:
			destroy_notify(event);
			break;
		case XCB_UNMAP_NOTIFY:
			unmap_notify(event);
			break;
		case XCB_ENTER_NOTIFY:
			enter_notify(event);
			break;
		case XCB_KEY_PRESS:
			key_press(event);
			break;
		}
	}
}
