#include "util.h"
#include "window.h"
#include "wm.h"
#include "workspace.h"

#include <xcb/xcb.h>
#include <xcb/xproto.h>

static void map_request(xcb_generic_event_t *gevent)
{
	xcb_map_request_event_t *e = (xcb_map_request_event_t *)gevent;
	map_ws(&workspace, e->window);
	xcb_flush(conn);
	log_message("MAP_REQUEST:\t%d\n", workspace.workspaces[workspace.current].subsidiaries.size);
}

static void destroy_notify(xcb_generic_event_t *gevent)
{
	xcb_destroy_notify_event_t *e = (xcb_destroy_notify_event_t *)gevent;
	unmap_ws(&workspace, e->window);
	xcb_flush(conn);
	log_message("UNMAP_NOTIFY:\t%d\n", workspace.workspaces[workspace.current].subsidiaries.size);
}

static void unmap_notify(xcb_generic_event_t *gevent)
{
	xcb_unmap_notify_event_t *e = (xcb_unmap_notify_event_t *)gevent;
	if (!window_exists(e->window)) {
		return;
	}
	unmap_ws(&workspace, e->window);
	xcb_flush(conn);
	log_message("UNMAP_NOTIFY:\t%d\n", workspace.workspaces[workspace.current].subsidiaries.size);
}

static void enter_notify(xcb_generic_event_t *gevent)
{
	xcb_enter_notify_event_t *e = (xcb_enter_notify_event_t *)gevent;
	if (e->mode == XCB_NOTIFY_MODE_NORMAL) {
		xcb_set_input_focus(conn, XCB_INPUT_FOCUS_PARENT, e->event, XCB_CURRENT_TIME);
	}

	xcb_get_geometry_cookie_t c = xcb_get_geometry(conn, e->event);
	xcb_get_geometry_reply_t *r = xcb_get_geometry_reply(conn, c, NULL);

	log_message("width: %d, height: %d\n", r->width, r->height);
	xcb_flush(conn);
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
		}
	}
}
