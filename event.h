#ifndef WM_EVENT_H
#define WM_EVENT_H

#include <xcb/xcb.h>
#include <xcb/xcb_event.h>

void handle_event(xcb_generic_event_t *);

#endif
