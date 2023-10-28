#ifndef WM_WINDOWS_H__
#define WM_WINDOWS_H__

#include <stdint.h>
#include <xcb/xcb.h>

void log_message(const char *format, ...);

typedef struct {
    xcb_window_t *data;
    size_t size;
    size_t capacity;
} Windows;

void initialize_windows(Windows *);
void free_windows(Windows *);
int window_exists(Windows *, xcb_window_t);
void add_window(Windows *, xcb_window_t);
void remove_window_at(Windows *, size_t);
void remove_window(Windows *, xcb_window_t);

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
void remove_from_workspace(Workspace *, xcb_window_t);
void promote_subsidiary(Workspace *);

/* resize window, given positions: x, y, w, h */
void resize_window(xcb_connection_t *, xcb_window_t, int, int, int, int);

#endif
