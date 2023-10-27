#include <xcb/xcb.h>

void start_xcb(xcb_connection_t **);
void stop_xcb(xcb_connection_t **);

void get_screen(xcb_connection_t *, xcb_screen_t **);
void modify_event_mask(xcb_window_t, uint32_t);
