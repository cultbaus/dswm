#include "dswm.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <xcb/xcb_keysyms.h>

xcb_key_symbols_t *keysyms;
xcb_connection_t *conn;
xcb_screen_t *screen;

static void window_manager_start(WindowManager *);
static void window_manager_initialize(WindowManager *, xcb_connection_t **, xcb_screen_t **, xcb_key_symbols_t **);
static void window_manager_destroy(WindowManager *, xcb_connection_t **);
static void window_manager_event_map(WindowManager *, xcb_generic_event_t *);
static void window_manager_event_unmap(WindowManager *, xcb_generic_event_t *);
static void window_manager_event_destroy(WindowManager *, xcb_generic_event_t *);
static void window_manager_event_enter(WindowManager *, xcb_generic_event_t *);
static void window_manager_event_key_press(WindowManager *, xcb_generic_event_t *);

int
main(void)
{
  WindowManager *wm = (WindowManager *)malloc(sizeof(WindowManager));

  window_manager_initialize(wm, &conn, &screen, &keysyms);
  window_manager_start(wm);
  window_manager_destroy(wm, &conn);
}

static void
window_manager_start(WindowManager *wm)
{
  xcb_generic_event_t *event;

  uint32_t root_mask = {ROOT_MASK};
  xcb_change_window_attributes(conn, screen->root, XCB_CW_EVENT_MASK, &root_mask);
  xcb_flush(conn);

  while ((event = xcb_wait_for_event(conn)))
  {
    switch (event->response_type & ~0x80)
    {
    case XCB_MAP_REQUEST:
      window_manager_event_map(wm, event);
      break;
    case XCB_UNMAP_NOTIFY:
      window_manager_event_unmap(wm, event);
      break;
    case XCB_DESTROY_NOTIFY:
      window_manager_event_destroy(wm, event);
      break;
    case XCB_ENTER_NOTIFY:
      window_manager_event_enter(wm, event);
      break;
    case XCB_KEY_PRESS:
      window_manager_event_key_press(wm, event);
      break;
    }
    free(event);
  }
}

static void
window_manager_initialize(WindowManager *wm, xcb_connection_t **conn, xcb_screen_t **screen, xcb_key_symbols_t **keysyms)
{
  // Initialize connection to X
  *conn = xcb_connect(NULL, NULL);
  if (xcb_connection_has_error(*conn))
    exit(1);

  // Get primary screen
  const xcb_setup_t *setup = xcb_get_setup(*conn);
  if (!setup)
    exit(2);
  *screen = xcb_setup_roots_iterator(setup).data;
  if (!*screen)
    exit(3);

  // Configure key grabs
  *keysyms = xcb_key_symbols_alloc(*conn);
  for (size_t i = 0; i < sizeof(keys) / sizeof(Key); i++)
  {
    xcb_keycode_t *keycodes = xcb_key_symbols_get_keycode(*keysyms, keys[i].keysym);
    for (xcb_keycode_t *keycode = keycodes; keycode && *keycode != XCB_NO_SYMBOL; keycode++)
    {
      xcb_grab_key(*conn, 0, (*screen)->root, keys[i].mod, *keycode, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
    }
    free(keycodes);
  }

  xcb_font_t font = xcb_generate_id(*conn);
  xcb_cursor_t cursor = xcb_generate_id(*conn);

  xcb_open_font(*conn, font, strlen("cursor"), "cursor");
  xcb_create_glyph_cursor(*conn, cursor, font, font, 68, 68 + 1, 65535, 65535, 65535, 0, 0, 0);

  xcb_change_window_attributes(*conn, (*screen)->root, XCB_CW_CURSOR, &cursor);

  // Initialize workspaces
  wm->current_workspace = DEFAULT_WORKSPACE;
  for (uint_fast8_t i = 0; i < MAX_WORKSPACES; i++)
  {
    wm->workspaces[i] = malloc(sizeof(Workspace));
    workspace_initialize(wm->workspaces[i]);
  }
}

static void
window_manager_destroy(WindowManager *wm, xcb_connection_t **conn)
{
  for (uint_fast8_t i = 0; i < MAX_WORKSPACES; i++)
  {
    workspace_destroy(wm->workspaces[i]);
    free(wm->workspaces[i]);
    wm->workspaces[i] = NULL;
  }
  free(wm);

  if (*conn)
    xcb_disconnect(*conn);
}

static void
window_manager_event_map(WindowManager *wm, xcb_generic_event_t *event)
{
  xcb_map_request_event_t *e = (xcb_map_request_event_t *)event;

  if (!window_exists(e->window))
    return;

  if (SLOPPY_FOCUS)
    window_set_sloppy_focus(e->window);

  if (BORDER_SIZE)
    window_set_border_size(e->window);

  if (BORDER_COLOR)
    window_set_border_color(e->window);

  workspace_add(wm->workspaces[wm->current_workspace], e->window);

  xcb_map_window(conn, e->window);
  xcb_flush(conn);

  if (workspace_contains(wm->workspaces[wm->current_workspace], e->window))
    workspace_focus(wm->workspaces[wm->current_workspace], e->window);
}

static void
window_manager_event_unmap(WindowManager *wm, xcb_generic_event_t *event)
{
  xcb_unmap_notify_event_t *e = (xcb_unmap_notify_event_t *)event;
  if (!window_exists(e->window))
    return;
  workspace_remove(wm->workspaces[wm->current_workspace], e->window);
}

static void
window_manager_event_destroy(WindowManager *wm, xcb_generic_event_t *event)
{
  xcb_destroy_notify_event_t *e = (xcb_destroy_notify_event_t *)event;

  workspace_remove(wm->workspaces[wm->current_workspace], e->window);

  xcb_unmap_window(conn, e->window);
  xcb_destroy_window(conn, e->window);
  xcb_flush(conn);

  if (workspace_contains(wm->workspaces[wm->current_workspace], e->window))
  {
    Workspace *current = wm->workspaces[wm->current_workspace];
    workspace_focus(current, current->windows->data[current->windows->size - 1]);
  }
}

static void
window_manager_event_enter(WindowManager *wm, xcb_generic_event_t *event)
{
  xcb_enter_notify_event_t *e = (xcb_enter_notify_event_t *)event;

  if (!(e->mode == XCB_NOTIFY_MODE_NORMAL))
    return;

  xcb_set_input_focus(conn, XCB_INPUT_FOCUS_PARENT, e->event, XCB_CURRENT_TIME);
  xcb_flush(conn);
}

static void
window_manager_event_key_press(WindowManager *wm, xcb_generic_event_t *event)
{
  xcb_key_press_event_t *e = (xcb_key_press_event_t *)event;
  for (unsigned int i = 0; i < sizeof(keys) / sizeof(Key); i++)
  {
    xcb_keysym_t keysym = xcb_key_symbols_get_keysym(keysyms, e->detail, 0);
    if (keys[i].mod == e->state && keys[i].keysym == keysym)
    {
      keys[i].func(wm, &keys[i].arg);
      break;
    }
  }
}

void
window_manager_send(WindowManager *wm, Arg *arg)
{
  xcb_get_input_focus_cookie_t cookie = xcb_get_input_focus(conn);
  xcb_get_input_focus_reply_t *reply = xcb_get_input_focus_reply(conn, cookie, NULL);

  if (reply)
  {
    xcb_unmap_window(conn, reply->focus);

    Workspace *next = wm->workspaces[arg->ui];
    workspace_add(next, reply->focus);
    workspace_remove(wm->workspaces[wm->current_workspace], reply->focus);

    free(reply);

    xcb_flush(conn);
  }
}

void
window_manager_move(WindowManager *wm, Arg *arg)
{
  if (wm->current_workspace == arg->ui)
    return;

  Workspace *current = wm->workspaces[wm->current_workspace];
  workspace_unmap_all(current);

  wm->current_workspace = arg->ui;

  Workspace *next = wm->workspaces[wm->current_workspace];
  workspace_map_all(next);

  xcb_flush(conn);
}

void
window_manager_quit(WindowManager *wm, Arg *arg)
{
  exit(arg->ui);
}

void
window_manager_kill(WindowManager *wm, Arg *arg)
{
  window_kill_focus();
}

void
window_manager_prev(WindowManager *wm, Arg *arg)
{
  Workspace *current = wm->workspaces[wm->current_workspace];
  workspace_focus_prev(current);
}

void
window_manager_next(WindowManager *wm, Arg *arg)
{

  Workspace *current = wm->workspaces[wm->current_workspace];
  workspace_focus_next(current);
}

void
window_manager_spawn(WindowManager *wm, Arg *arg)
{
  if (fork())
    return;

  if (conn)
    close(xcb_get_file_descriptor(conn));

  execvp(((char **)arg->v)[0], arg->v);
}
