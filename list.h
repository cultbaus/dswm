#ifndef WM_LIST_H
#define WM_LIST_H

#include <xcb/xcb.h>

typedef struct List
{
  unsigned int *data;
  unsigned int size;
  unsigned int capacity;
} List;

void list_initialize(List *);
void list_destroy(List *);
void list_add(List *, xcb_window_t);
void list_remove(List *, xcb_window_t);
xcb_window_t list_next(List *, xcb_window_t);
xcb_window_t list_prev(List *, xcb_window_t);
unsigned int list_contains(List *, xcb_window_t);

extern xcb_connection_t *conn;
extern xcb_screen_t *screen;

#endif // WM_LIST_H
