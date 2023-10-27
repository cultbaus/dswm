#ifndef WM_WINDOWS_H__
#define WM_WINDOWS_H__

#include <stdint.h>
#include <xcb/xcb.h>

typedef struct {
    xcb_window_t *data;
    size_t size;
    size_t capacity;
} Windows;

void initialize_windows(Windows *);
void free_windows(Windows *);
void add_window(Windows *, xcb_window_t);
void remove_window_at(Windows *, size_t);

typedef struct {
    xcb_connection_t *conn;
    xcb_screen_t *screen;
    xcb_window_t primary;
    Windows subsidiaries;
    size_t num;
} Workspace;

void initialize_workspace(Workspace *, xcb_connection_t *, xcb_screen_t *);
void free_workspace(Workspace *);
void add_to_workspace(Workspace *, xcb_window_t);
void promote_subsidiary(Workspace *, xcb_window_t);

/* resize window, given positions: x, y, w, h */
void resize_window(xcb_connection_t *, xcb_window_t, int, int, int, int);

#endif
