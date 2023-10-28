#include "wm.h"

#include "event.h"
#include "window.h"
#include "workspace.h"

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>

xcb_connection_t *conn;
xcb_screen_t *screen;
xcb_window_t root;

workspace_t workspace;

static void start_xcb(xcb_connection_t **conn)
{
    *conn = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(*conn))
        errx(1, "unable to connect to the x server");
}

static void stop_xcb(xcb_connection_t **conn)
{
    if (*conn)
        xcb_disconnect(*conn);
}

static void get_screen(xcb_connection_t *conn, xcb_screen_t **screen)
{
    const xcb_setup_t *setup = xcb_get_setup(conn);
    if (!setup) {
        errx(1, "unable to retrieve setup");
    }
    *screen = xcb_setup_roots_iterator(setup).data;
    if (!*screen) {
        errx(2, "unable to retrieve roots iterator");
    }
}

static void set_mask(xcb_window_t win, uint32_t mask)
{
    uint32_t value_mask = {mask};
    xcb_change_window_attributes(conn, win, XCB_CW_EVENT_MASK, &value_mask);
    xcb_flush(conn);
}

int main(void)
{
    xcb_generic_event_t *event;

    start_xcb(&conn);
    get_screen(conn, &screen);
    set_mask(screen->root, ROOT_MASK);
    setup_ws(&workspace);

    while ((event = xcb_wait_for_event(conn))) {
        handle_event(event);
        free(event);
    }

    free_ws(&workspace);
    stop_xcb(&conn);

    return 0;
}
