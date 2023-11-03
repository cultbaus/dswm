#include "window.h"
#include "config.h"

#include <stdlib.h>

void
window_move(xcb_window_t win, uint16_t x, uint16_t y)
{
  uint32_t values[] = {x, y};
  xcb_configure_window(conn, win, MOVEMENT_MASK, values);
}

void
window_resize(xcb_window_t win, uint16_t w, uint16_t h)
{
  uint32_t values[] = {w, h};
  xcb_configure_window(conn, win, POSITION_MASK, values);
}

void
window_set_sloppy_focus(unsigned int win)
{
  uint32_t values = {XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_ENTER_WINDOW};
  xcb_change_window_attributes(conn, win, XCB_CW_EVENT_MASK, &values);
}

void
window_set_border_size(xcb_window_t win)
{
  uint32_t values = {BORDER_SIZE};
  xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_BORDER_WIDTH, &values);
}

void
window_set_border_color(xcb_window_t win)
{
  uint32_t values = {BORDER_COLOR};
  xcb_change_window_attributes(conn, win, XCB_CW_BORDER_PIXEL, &values);
}

void
window_kill_focus(void)
{
  xcb_get_input_focus_cookie_t cookie = xcb_get_input_focus(conn);
  xcb_get_input_focus_reply_t *reply = xcb_get_input_focus_reply(conn, cookie, NULL);
  if (reply)
  {
    xcb_kill_client(conn, reply->focus);
    xcb_destroy_window(conn, reply->focus);
    xcb_flush(conn);
    free(reply);
  }
}

void
window_set_focus(unsigned int win)
{
  xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT, win, XCB_CURRENT_TIME);
  xcb_flush(conn);
}

unsigned int
window_exists(unsigned int win)
{
  xcb_generic_error_t *err;
  free(xcb_query_tree_reply(conn, xcb_query_tree(conn, win), &err));
  if (err != NULL)
  {
    free(err);
    return 0;
  }
  return 1;
}

xcb_get_geometry_reply_t *
window_get_geometry(xcb_window_t win)
{
  xcb_get_geometry_cookie_t cookie = xcb_get_geometry(conn, win);
  return xcb_get_geometry_reply(conn, cookie, NULL);
}
