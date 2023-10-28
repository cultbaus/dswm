#include "windows.h"

#include "config.h"

#include <err.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xproto.h>

void log_message(const char *format, ...)
{
    FILE *logfile = fopen("log", "a");

    va_list args;
    va_start(args, format);
    vfprintf(logfile, format, args);
    va_end(args);
    fclose(logfile);
}

void initialize_windows(Windows *windows)
{
    windows->data = NULL;
    windows->size = 0;
    windows->capacity = 0;
}

void free_windows(Windows *windows)
{
    free(windows->data);
    windows->data = NULL;
    windows->size = windows->capacity = 0;
}

int window_exists(Windows *windows, xcb_window_t window)
{
    for (size_t i = 0; i < windows->size; i++) {
        if (windows->data[i] == window) {
            return 1;
        }
    }
    return 0;
}

void add_window(Windows *windows, xcb_window_t window)
{
    if (window_exists(windows, window)) {
        return;
    }
    if (windows->size == windows->capacity) {
        windows->capacity = windows->capacity == 0 ? 1 : windows->capacity * 2;
        windows->data = realloc(windows->data, windows->capacity * sizeof(xcb_window_t));
        if (!windows->data) {
            errx(1, "failed to allocate memory for subsidiary windows");
        }
    }
    windows->data[windows->size++] = window;
}
void remove_window_at(Windows *windows, size_t index)
{
    if (index < windows->size - 1) {
        memmove(&windows->data[index], &windows->data[index + 1], (windows->size - index - 1) * sizeof(xcb_window_t));
    }
    windows->size--;
}

void remove_window(Windows *windows, xcb_window_t window)
{
    for (size_t i = 0; i < windows->size; i++) {
        if (windows->data[i] == window) {
            remove_window_at(windows, i);
            return;
        }
    }
}

void initialize_workspace(Workspace *workspace, xcb_connection_t *conn, xcb_screen_t *screen)
{
    workspace->screen = screen;
    workspace->conn = conn;
    workspace->primary = XCB_NONE;
    initialize_windows(&workspace->subsidiaries);
}

void free_workspace(Workspace *workspace) { free_windows(&workspace->subsidiaries); }

void add_to_workspace(Workspace *workspace, xcb_window_t window)
{
    if (workspace->primary == XCB_NONE) {
        workspace->primary = window;
        resize_window(workspace->conn, window, gap_size, gap_size,
                      workspace->screen->width_in_pixels - ((gap_size * 2) - border_size),
                      workspace->screen->height_in_pixels - ((gap_size * 2) - border_size));
    } else {
        add_window(&workspace->subsidiaries, window);
    }
}

void remove_from_workspace(Workspace *workspace, xcb_window_t window)
{
    if (workspace->primary == window) {
        promote_subsidiary(workspace);
    }
    remove_window(&workspace->subsidiaries, window);
}

void promote_subsidiary(Workspace *workspace)
{
    if (workspace->subsidiaries.size == 0) {
        return;
    }
    workspace->primary = workspace->subsidiaries.data[0];
    remove_window_at(&workspace->subsidiaries, 0);
}

void resize_window(xcb_connection_t *conn, xcb_window_t window, int x, int y, int w, int h)
{
    uint32_t m = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    uint32_t position[4] = {x, y, w, h};
    xcb_configure_window(conn, window, m, position);
}
