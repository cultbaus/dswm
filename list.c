#include "list.h"

#include <stdlib.h>
#include <string.h>

static unsigned int list_contains(List *, unsigned int);
static void list_resize(List *);

void
list_initialize(List *list)
{
  list->data = NULL;
  list->size = 0;
  list->capacity = 0;
}

void
list_destroy(List *list)
{
  free(list->data);

  list->capacity = 0;
  list->size = 0;
  list->data = NULL;
}

void
list_add(List *list, xcb_window_t win)
{
  if (list_contains(list, win))
    return;

  if (list->size == list->capacity)
    list_resize(list);

  list->data[list->size++] = win;
}

void
list_remove(List *list, xcb_window_t win)
{
  for (unsigned int i = 0; i < list->size; i++)
  {
    if (list->data[i] == win)
    {
      if (i < list->size - 1)
      {
        memmove(&list->data[i], &list->data[i + 1], (list->size - i - 1) * sizeof(xcb_window_t));
      }
      list->size--;
      return;
    }
  }
}

xcb_window_t
list_next(List *list, xcb_window_t win)
{
  for (unsigned int i = 0; i < list->size; i++)
  {
    if (list->data[i] == win && i < list->size - 1)
      return list->data[i + 1];
  }
  return list->data[0];
}

xcb_window_t
list_prev(List *list, xcb_window_t win)
{
  for (unsigned int i = 0; i < list->size; i++)
  {
    if (list->data[i] == win && i >= 1)
      return list->data[i - 1];
  }
  return list->data[list->size - 1];
}

static unsigned int
list_contains(List *list, xcb_window_t win)
{
  for (unsigned int i = 0; i < list->size; i++)
  {
    if (list->data[i] == win)
      return 1;
  }
  return 0;
}

static void
list_resize(List *list)
{
  list->capacity = list->capacity == 0 ? 1 : list->capacity * 2;
  list->data = realloc(list->data, list->capacity * sizeof(unsigned int));

  if (!list->data) // Re-allocation failed, panic
    exit(1);
}
