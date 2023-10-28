#include "window.h"

#include "wm.h"

#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>

int window_exists(xcb_window_t win)
{
	xcb_generic_error_t *err;
	free(xcb_query_tree_reply(conn, xcb_query_tree(conn, win), &err));
	if (err != NULL) {
		free(err);
		return 0;
	}
	return 1;
}

void window_move(xcb_window_t win, int16_t x, int16_t y)
{
	uint32_t values[] = { x, y };
	xcb_configure_window(conn, win, XCB_CONFIG_X_Y, values);
}

void window_resize(xcb_window_t win, int16_t w, int16_t h)
{
	uint32_t values[] = { w, h };
	xcb_configure_window(conn, win, XCB_CONFIG_W_H, values);
}

void window_move_resize(xcb_window_t win, int16_t x, int16_t y, int16_t w, int16_t h)
{
	uint32_t values[] = { x, y, w, h };
	xcb_configure_window(conn, win, XCB_CONFIG_X_Y_W_H, values);
}

void window_border_width(xcb_window_t win, uint32_t bw)
{
	uint32_t values[] = { bw };
	xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_BORDER_WIDTH, values);
}

void window_border_color(xcb_window_t win, uint32_t color)
{
	uint32_t values[] = { color };
	xcb_change_window_attributes(conn, win, XCB_CW_BORDER_PIXEL, values);
}

void window_sloppy_focus(xcb_window_t win)
{
	uint32_t values[] = { CLIENT_MASK | XCB_EVENT_MASK_ENTER_WINDOW };
	xcb_change_window_attributes(conn, win, XCB_CW_EVENT_MASK, values);
}

void setup_windows(windows_t *windows)
{
	windows->list = NULL;
	windows->size = 0;
	windows->capacity = 0;
}

void free_windows(windows_t *windows)
{
	free(windows->list);
	windows->list = NULL;
	windows->size = windows->capacity = 0;
}

int exists_in_windows(windows_t *windows, xcb_window_t window)
{
	for (size_t i = 0; i < windows->size; i++) {
		if (windows->list[i] == window) {
			return 1;
		}
	}
	return 0;
}

void remove_from_windows(windows_t *windows, xcb_window_t window)
{
	for (size_t i = 0; i < windows->size; i++) {
		if (windows->list[i] == window) {
			if (i < windows->size - 1) {
				memmove(&windows->list[i], &windows->list[i + 1],
					(windows->size - i - 1) * sizeof(xcb_window_t));
			}
			windows->size--;
			return;
		}
	}
}

void add_to_windows(windows_t *windows, xcb_window_t window)
{
	if (exists_in_windows(windows, window)) {
		return;
	}
	if (windows->size == windows->capacity) {
		windows->capacity = windows->capacity == 0 ? 1 : windows->capacity * 2;
		windows->list = realloc(windows->list, windows->capacity * sizeof(xcb_window_t));
		if (!windows->list) {
			exit(1);
		}
	}
	windows->list[windows->size++] = window;
}
