#include <xcb/xproto.h>

void enter_notify(xcb_connection_t *conn, xcb_generic_event_t *event)
{
    xcb_enter_notify_event_t *en = (xcb_enter_notify_event_t *)event;
    if (en->mode == XCB_NOTIFY_MODE_NORMAL) {
        xcb_set_input_focus(conn, XCB_INPUT_FOCUS_PARENT, en->event, XCB_CURRENT_TIME);
    }
}
