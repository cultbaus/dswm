#ifndef WM_WINDOW_H
#define WM_WINDOW_H

#include <xcb/xcb.h>

#define ROOT_MASK (                        \
    XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | \
    XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |   \
    XCB_EVENT_MASK_STRUCTURE_NOTIFY |      \
    XCB_EVENT_MASK_FOCUS_CHANGE |          \
    XCB_EVENT_MASK_KEY_PRESS)

#define CLIENT_MASK (                \
    XCB_EVENT_MASK_PROPERTY_CHANGE | \
    XCB_EVENT_MASK_FOCUS_CHANGE)

#define MOVEMENT_MASK (XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y)
#define POSITION_MASK (XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT)

void window_move(xcb_window_t, uint16_t, uint16_t);
void window_resize(xcb_window_t, uint16_t, uint16_t);
void window_set_sloppy_focus(unsigned int);
void window_set_border_size(unsigned int);
void window_set_border_color(unsigned int);
void window_kill_focus(void);
void window_set_focus(unsigned int);
unsigned int window_exists(unsigned int);
xcb_get_geometry_reply_t *window_get_geometry(unsigned int);

extern xcb_connection_t *conn;
extern xcb_screen_t *screen;

#endif // WM_WINDOW_H
