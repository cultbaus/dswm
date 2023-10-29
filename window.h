#ifndef WM_WINDOWS_H__
#define WM_WINDOWS_H__

#include <stdint.h>
#include <xcb/xcb.h>

#define XCB_CONFIG_X_Y (XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y)
#define XCB_CONFIG_W_H (XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT)
#define XCB_CONFIG_X_Y_W_H \
	(XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT)

int window_exists(xcb_window_t);
void window_move(xcb_window_t, int16_t, int16_t);
void window_resize(xcb_window_t, int16_t, int16_t);
void window_move_resize(xcb_window_t, int16_t, int16_t, int16_t, int16_t);
void window_border_width(xcb_window_t, uint32_t);
void window_border_color(xcb_window_t, uint32_t);
void window_sloppy_focus(xcb_window_t);
void window_kill();

typedef struct {
	xcb_window_t *list;
	size_t size;
	size_t capacity;
} windows_t;

void setup_windows(windows_t *);
void free_windows(windows_t *);
void remove_from_windows(windows_t *, xcb_window_t);
void add_to_windows(windows_t *, xcb_window_t);

extern xcb_connection_t *conn;
extern xcb_screen_t *screen;

#endif
