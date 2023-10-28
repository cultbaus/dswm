#ifndef WM_WORKSPACE_H
#define WM_WORKSPACE_H

#include "config.h"
#include "window.h"

#include <xcb/xcb.h>

typedef struct {
    xcb_window_t primary;
    windows_t subsidiaries;
} space_t;

typedef struct {
    space_t workspaces[MAX_WORKSPACES];
    size_t current;
} workspace_t;

void setup_ws(workspace_t *);
void free_ws(workspace_t *);
void add_to_ws(workspace_t *, xcb_window_t);
void remove_from_ws(workspace_t *, xcb_window_t);

void map_ws(workspace_t *, xcb_window_t);
void unmap_ws(workspace_t *, xcb_window_t);

extern workspace_t workspace;
extern xcb_screen_t *screen;
extern xcb_connection_t *conn;

#endif
