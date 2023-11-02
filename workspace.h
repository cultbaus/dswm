#ifndef WM_WORKSPACE_H
#define WM_WORKSPACE_H

#include "config.h"
#include "list.h"
#include "window.h"

typedef struct Workspace
{
  int focus;
  List *windows;
} Workspace;

void workspace_initialize(Workspace *);
void workspace_destroy(Workspace *);
void workspace_add(Workspace *, xcb_window_t);
void workspace_remove(Workspace *, xcb_window_t);
void workspace_map_all(Workspace *);
void workspace_unmap_all(Workspace *);

extern xcb_connection_t *conn;
extern xcb_screen_t *screen;

#endif // WM_WORKSPACE_H
