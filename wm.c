#include "event.h"

#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

static xcb_connection_t *conn;
static xcb_screen_t *screen;

static uint32_t root_m = XCB_EVENT_MASK_NO_EVENT | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY;
static uint32_t win_m = XCB_EVENT_MASK_NO_EVENT | XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_STRUCTURE_NOTIFY
                        | XCB_EVENT_MASK_FOCUS_CHANGE;

void start_xcb(xcb_connection_t **conn)
{
    *conn = xcb_connect(NULL, NULL);

    if (xcb_connection_has_error(*conn))
        errx(1, "unable to connect to the X server");
}

void stop_xcb(xcb_connection_t **conn)
{
    if (*conn)
        xcb_disconnect(*conn);
}

void get_screen(xcb_connection_t *conn, xcb_screen_t **screen)
{
    if (!conn || !screen) {
        errx(1, "received null pointer for screen or connection");
    }

    const xcb_setup_t *setup = xcb_get_setup(conn);
    if (!setup) {
        errx(2, "unable to retrieve setup");
    }

    *screen = xcb_setup_roots_iterator(setup).data;
    if (!*screen) {
        errx(3, "unable to retrieve roots iterator");
    }
}

void modify_event_mask(xcb_window_t win, uint32_t mask)
{
    uint32_t value_mask = {mask};

    /* Register events we are interested in */
    xcb_change_window_attributes(conn, win, XCB_CW_EVENT_MASK, &value_mask);

    /* Send events to the X server */
    xcb_flush(conn);
}

int main(void)
{
    xcb_generic_event_t *event;
    xcb_create_notify_event_t *create_notify;

    start_xcb(&conn);
    get_screen(conn, &screen);

    modify_event_mask(screen->root, root_m);

    while ((event = xcb_wait_for_event(conn))) {
        switch (event->response_type & ~0x80) {
        case XCB_CREATE_NOTIFY:
            create_notify = (xcb_create_notify_event_t *)event;
            modify_event_mask(create_notify->window, win_m);
        case XCB_ENTER_NOTIFY:
            enter_notify(conn, event);
        }
        free(event);
    }

    stop_xcb(&conn);
    return 0;
}
