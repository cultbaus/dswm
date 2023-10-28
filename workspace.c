#include "workspace.h"

#include "config.h"
#include "util.h"
#include "window.h"

#include <stdlib.h>
#include <xcb/xcb.h>

static void setup_space(space_t *space)
{
    space->primary = XCB_NONE;
    setup_windows(&space->subsidiaries);
}

static void free_space(space_t *space) { free_windows(&space->subsidiaries); }

static void arrange(workspace_t *ws)
{
    space_t *s = &ws->workspaces[ws->current];
    uint32_t w = screen->width_in_pixels;
    uint32_t h = screen->height_in_pixels;
    uint32_t y = 0;

    if (s->primary != XCB_NONE && s->subsidiaries.size >= 1) {
        window_move_resize(s->primary, 0, 0, w / 2, h);
        for (size_t i = 0; i < s->subsidiaries.size; i++) {
            window_move_resize(s->subsidiaries.list[i], w / 2, y, w / 2, h / s->subsidiaries.size);
            y += h / s->subsidiaries.size;
        }
    }
    if (s->primary != XCB_NONE && s->subsidiaries.size == 0) {
        window_move_resize(s->primary, 0, 0, w, h);
    }
}

void setup_ws(workspace_t *ws)
{
    ws->current = 0;
    for (size_t i = 0; i < MAX_WORKSPACES; i++) {
        setup_space(&ws->workspaces[i]);
    }
}

void free_ws(workspace_t *wss)
{
    for (size_t i = 0; i < MAX_WORKSPACES; i++) {
        free_space(&wss->workspaces[i]);
    }
}

void add_to_ws(workspace_t *ws, xcb_window_t window)
{
    space_t *s = &ws->workspaces[ws->current];
    if (s->primary == XCB_NONE) {
        s->primary = window;
        return;
    }
    if (s->primary != window) {
        add_to_windows(&s->subsidiaries, window);
    }
}

void remove_from_ws(workspace_t *ws, xcb_window_t window)
{
    space_t *s = &ws->workspaces[ws->current];
    if (s->primary == window) {
        if (s->subsidiaries.size > 0) {
            s->primary = s->subsidiaries.list[0];
            remove_from_windows(&s->subsidiaries, s->subsidiaries.list[0]);
        } else {
            s->primary = XCB_NONE;
        }
        return;
    }
    remove_from_windows(&s->subsidiaries, window);
}

void map_ws(workspace_t *ws, xcb_window_t win)
{
    window_border_width(win, border_size);
    window_border_color(win, border_color);
    xcb_flush(conn);

    add_to_ws(ws, win);
    arrange(ws);
    xcb_map_window(conn, win);
}

void unmap_ws(workspace_t *ws, xcb_window_t win)
{
    remove_from_ws(ws, win);
    arrange(ws);
    xcb_unmap_window(conn, win);
}
