#include "workspace.h"

#include "config.h"
#include "util.h"
#include "window.h"

#include "wm.h"
#include <stdlib.h>
#include <xcb/xcb.h>

static void setup_space(space_t *space)
{
	space->primary = XCB_NONE;
	setup_windows(&space->subsidiaries);
}

static void free_space(space_t *space)
{
	free_windows(&space->subsidiaries);
}

static void arrange(workspace_t *ws)
{
	space_t *s = &ws->workspaces[ws->current];

	uint32_t c_gap = gap_size - (2 * border_size);

	uint32_t f_width = screen->width_in_pixels;

	uint32_t gf_width = screen->width_in_pixels - (c_gap * 2);
	uint32_t gf_height = screen->height_in_pixels - (c_gap * 2);

	uint32_t h_width = f_width / 2;

	if (s->primary != XCB_NONE && s->subsidiaries.size >= 1) {
		uint32_t p_width = h_width - (c_gap * 3) / 2;
		uint32_t s_x_offset = (c_gap * 2) + p_width;

		window_move_resize(s->primary, c_gap, c_gap, p_width, gf_height);

		uint32_t s_height = (gf_height - (s->subsidiaries.size - 1) * gap_size) / s->subsidiaries.size;
		uint32_t y = c_gap;

		for (size_t i = 0; i < s->subsidiaries.size; i++) {
			window_move_resize(s->subsidiaries.list[i], s_x_offset, y, p_width, s_height);
			y += s_height + gap_size;
		}
	}

	if (s->primary != XCB_NONE && s->subsidiaries.size == 0) {
		window_move_resize(s->primary, c_gap, c_gap, gf_width, gf_height);
	}
}

void setup_ws(workspace_t *ws)
{
	ws->current = 0;
	for (size_t i = 0; i < MAX_WORKSPACES; i++) {
		setup_space(&ws->workspaces[i]);
	}
}

void free_ws(workspace_t *wss)
{
	for (size_t i = 0; i < MAX_WORKSPACES; i++) {
		free_space(&wss->workspaces[i]);
	}
}

void add_to_ws(workspace_t *ws, xcb_window_t window)
{
	space_t *s = &ws->workspaces[ws->current];
	if (s->primary == XCB_NONE) {
		s->primary = window;
		return;
	}
	if (s->primary != window) {
		add_to_windows(&s->subsidiaries, window);
	}
}

void remove_from_ws(workspace_t *ws, xcb_window_t window)
{
	space_t *s = &ws->workspaces[ws->current];
	if (s->primary == window) {
		if (s->subsidiaries.size > 0) {
			s->primary = s->subsidiaries.list[0];
			remove_from_windows(&s->subsidiaries, s->subsidiaries.list[0]);
		} else {
			s->primary = XCB_NONE;
		}
		return;
	}
	remove_from_windows(&s->subsidiaries, window);
}

void map_ws(workspace_t *ws, xcb_window_t win)
{
	window_border_width(win, border_size);
	window_border_color(win, border_color);
	window_sloppy_focus(win);
	xcb_flush(conn);

	add_to_ws(ws, win);
	arrange(ws);
	xcb_map_window(conn, win);
}

void unmap_ws(workspace_t *ws, xcb_window_t win)
{
	remove_from_ws(ws, win);
	arrange(ws);
	xcb_unmap_window(conn, win);
}
