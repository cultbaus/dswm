#ifndef WM_WINDOW_MANAGER_H
#define WM_WINDOW_MANAGER_H

#include "workspace.h"

#include <X11/keysym.h>
#include <xcb/xcb_keysyms.h>

typedef struct WindowManager
{
  Workspace *workspaces[MAX_WORKSPACES];
  unsigned int current_workspace;
} WindowManager;

typedef union Arg
{
  unsigned int ui;
  const void *v;
} Arg;

typedef struct Key
{
  uint16_t mod;
  xcb_keysym_t keysym;
  void (*func)(WindowManager *, Arg *);
  Arg arg;
} Key;

void window_manager_move(WindowManager *, Arg *);
void window_manager_send(WindowManager *, Arg *);
void window_manager_quit(WindowManager *, Arg *);
void window_manager_kill(WindowManager *, Arg *);
void window_manager_prev(WindowManager *, Arg *);
void window_manager_next(WindowManager *, Arg *);
void window_manager_spawn(WindowManager *, Arg *);

static const char *term[] = {TERMINAL, NULL};

static Key keys[] = {
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_l, window_manager_quit, {.ui = 0}},
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_q, window_manager_kill, {0}},
    {MOD_KEY, XK_0, window_manager_move, {.ui = 0}},
    {MOD_KEY, XK_1, window_manager_move, {.ui = 1}},
    {MOD_KEY, XK_2, window_manager_move, {.ui = 2}},
    {MOD_KEY, XK_3, window_manager_move, {.ui = 3}},
    {MOD_KEY, XK_4, window_manager_move, {.ui = 4}},
    {MOD_KEY, XK_5, window_manager_move, {.ui = 5}},
    {MOD_KEY, XK_6, window_manager_move, {.ui = 6}},
    {MOD_KEY, XK_7, window_manager_move, {.ui = 7}},
    {MOD_KEY, XK_8, window_manager_move, {.ui = 8}},
    {MOD_KEY, XK_9, window_manager_move, {.ui = 9}},
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_0, window_manager_send, {.ui = 0}},
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_1, window_manager_send, {.ui = 1}},
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_2, window_manager_send, {.ui = 2}},
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_3, window_manager_send, {.ui = 3}},
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_4, window_manager_send, {.ui = 4}},
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_5, window_manager_send, {.ui = 5}},
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_6, window_manager_send, {.ui = 6}},
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_7, window_manager_send, {.ui = 7}},
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_8, window_manager_send, {.ui = 8}},
    {MOD_KEY | XCB_MOD_MASK_SHIFT, XK_9, window_manager_send, {.ui = 9}},
    {MOD_KEY, XK_j, window_manager_next, {0}},
    {MOD_KEY, XK_k, window_manager_prev, {0}},
    {MOD_KEY, XK_Return, window_manager_spawn, {.v = term}},

};

#endif // WM_WINDOW_MANAGER_H
