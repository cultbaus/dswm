#include "event.h"
#include "windows.h"

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
                        | XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_EXPOSURE;

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

    xcb_change_window_attributes(conn, win, XCB_CW_EVENT_MASK, &value_mask);
    xcb_flush(conn);
}

int main(void)
{
    Workspace workspace;
    xcb_generic_event_t *event;

    start_xcb(&conn);
    get_screen(conn, &screen);

    modify_event_mask(screen->root, root_m);

    initialize_workspace(&workspace, conn, screen);

    while ((event = xcb_wait_for_event(conn))) {
        switch (event->response_type & ~0x80) {
        case XCB_CREATE_NOTIFY: {
            xcb_create_notify_event_t *cn = (xcb_create_notify_event_t *)event;

            modify_event_mask(cn->window, win_m);

            xcb_flush(conn);
            log_message("CREATE: Subsidiary size: %zu\n", workspace.subsidiaries.size);
        }
        case XCB_MAP_REQUEST: {
            xcb_map_request_event_t *mr = (xcb_map_request_event_t *)event;

            add_to_workspace(&workspace, mr->window);
            xcb_map_window(conn, mr->window);

            xcb_flush(conn);
            log_message("MAP: Subsidiary size: %zu\n", workspace.subsidiaries.size);
            break;
        }
        case XCB_DESTROY_NOTIFY: {
            xcb_destroy_notify_event_t *dn = (xcb_destroy_notify_event_t *)event;

            remove_from_workspace(&workspace, dn->window);

            xcb_flush(conn);
            log_message("DESTROY: Subsidiary size: %zu\n", workspace.subsidiaries.size);
            break;
        }
        case XCB_ENTER_NOTIFY: {
            xcb_enter_notify_event_t *en = (xcb_enter_notify_event_t *)event;

            if (en->mode == XCB_NOTIFY_MODE_NORMAL) {
                xcb_set_input_focus(conn, XCB_INPUT_FOCUS_PARENT, en->event, XCB_CURRENT_TIME);
            }

            xcb_flush(conn);
            break;
        }
        }
        free(event);
    }
    free_workspace(&workspace);

    stop_xcb(&conn);

    return 0;
}
