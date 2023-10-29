#ifndef WM_WORKSPACE_H
#define WM_WORKSPACE_H

#include "window.h"

#include <xcb/xcb.h>

#define MAX_WORKSPACES 10

typedef struct {
	xcb_window_t primary;
	windows_t subsidiaries;
} space_t;

typedef struct {
	space_t spaces[MAX_WORKSPACES];
	size_t current;
} workspace_t;

void setup_ws(workspace_t *);
void free_ws(workspace_t *);
void add_to_ws(space_t *, xcb_window_t);
void remove_from_ws(space_t *, xcb_window_t);

void map_ws(space_t *, xcb_window_t);
void unmap_ws(space_t *, xcb_window_t);

space_t *current_s(workspace_t *);
void switch_ws(workspace_t *);

extern workspace_t workspace;
extern xcb_screen_t *screen;
extern xcb_connection_t *conn;

#endif
