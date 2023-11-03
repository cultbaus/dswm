#include "workspace.h"

#include <stdlib.h>

static unsigned int workspace_contains_primary(Workspace *);
static unsigned int workspace_should_resize_primary(Workspace *);
static unsigned int workspace_should_reposition_primary(Workspace *);
static void workspace_resize_primary(Workspace *);
static unsigned int workspace_contains_secondaries(Workspace *);
static void workspace_resize_secondaries(Workspace *);

void
workspace_initialize(Workspace *ws)
{
  ws->focus = XCB_NONE;
  ws->windows = malloc(sizeof(List));
  list_initialize(ws->windows);
}

void
workspace_destroy(Workspace *ws)
{
  list_destroy(ws->windows);
  free(ws->windows);
}

void
workspace_add(Workspace *ws, xcb_window_t win)
{
  list_add(ws->windows, win);

  if (workspace_should_resize_primary(ws))
    workspace_resize_primary(ws);

  workspace_resize_secondaries(ws);

  ws->focus = win;
}

void
workspace_remove(Workspace *ws, xcb_window_t win)
{
  list_remove(ws->windows, win);

  if (workspace_should_resize_primary(ws))
    workspace_resize_primary(ws);

  workspace_resize_secondaries(ws);
}

void
workspace_map_all(Workspace *ws)
{
  for (unsigned int i = 0; i < ws->windows->size; i++)
    xcb_map_window(conn, ws->windows->data[i]);

  xcb_flush(conn);
}

void
workspace_unmap_all(Workspace *ws)
{
  for (unsigned int i = 0; i < ws->windows->size; i++)
    xcb_unmap_window(conn, ws->windows->data[i]);

  xcb_flush(conn);
}

void
workspace_focus(Workspace *ws, xcb_window_t win)
{
  window_set_focus(win);
  ws->focus = win;
}

void
workspace_focus_next(Workspace *ws)
{
  if (ws->windows->size == 1)
    return;

  xcb_window_t next = list_next(ws->windows, ws->focus);
  workspace_focus(ws, next);
}

void
workspace_focus_prev(Workspace *ws)
{
  if (ws->windows->size < 1)
    return;

  xcb_window_t prev = list_prev(ws->windows, ws->focus);
  workspace_focus(ws, prev);
}

static unsigned int
workspace_contains_primary(Workspace *ws)
{
  return ws->windows->size > 0;
}

static void
workspace_resize_primary(Workspace *ws)
{
  if (!workspace_contains_primary(ws))
    return;

  if (workspace_should_reposition_primary(ws))
    window_move(ws->windows->data[0], GAP_SIZE, GAP_SIZE);

  if (ws->windows->size == 1)
    window_resize(ws->windows->data[0], screen->width_in_pixels - (GAP_SIZE * 2), screen->height_in_pixels - (GAP_SIZE * 2));
  else
    window_resize(ws->windows->data[0], screen->width_in_pixels / 2 - (GAP_SIZE * 3) / 2, screen->height_in_pixels - (GAP_SIZE * 2));
}

static unsigned int
workspace_should_resize_primary(Workspace *ws)
{
  return ws->windows->size >= 1 ? 1 : 0;
}

static unsigned int
workspace_should_reposition_primary(Workspace *ws)
{
  xcb_get_geometry_reply_t *reply = window_get_geometry(ws->windows->data[0]);

  unsigned int should = reply->x != GAP_SIZE || reply->y != GAP_SIZE ? 1 : 0;
  free(reply);

  return should;
}

static unsigned int
workspace_contains_secondaries(Workspace *ws)
{
  return ws->windows->size > 1 ? 1 : 0;
}

static void
workspace_resize_secondaries(Workspace *ws)
{
  if (!workspace_contains_secondaries(ws))
    return;

  uint16_t const half_screen_width = screen->width_in_pixels / 2;
  uint16_t const half_gap_size = GAP_SIZE * 3 / 2;
  uint16_t const window_width = half_screen_width - half_gap_size;
  uint16_t const total_gap_height = (ws->windows->size - 2) * GAP_SIZE;
  uint16_t const usable_height = screen->height_in_pixels - (GAP_SIZE * 2);
  uint16_t const window_height = (usable_height - total_gap_height) / (ws->windows->size - 1);

  uint16_t y_offset = GAP_SIZE;

  for (unsigned int i = 1; i < ws->windows->size; i++)
  {
    uint16_t x_position = (GAP_SIZE * 3 / 2) + (half_screen_width - half_gap_size / 2);

    window_move(ws->windows->data[i], x_position, y_offset);
    window_resize(ws->windows->data[i], window_width, window_height);

    y_offset += window_height + GAP_SIZE;
  }
}
