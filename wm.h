#ifndef WM_WM_H__
#define WM_WM_H__

#include <xcb/xcb.h>

#define ROOT_MASK                                                                                                      \
	(XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_STRUCTURE_NOTIFY | \
	 XCB_EVENT_MASK_FOCUS_CHANGE)
#define CLIENT_MASK (XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_FOCUS_CHANGE)

extern xcb_connection_t *conn;
extern xcb_screen_t *screen;
extern xcb_window_t root;

#endif
