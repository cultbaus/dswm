#include "key.h"
#include "workspace.h"

#include <stdlib.h>

static void quit(args_u *);
static void kill_window(args_u *);

static void move_to_workspace(args_u *);
static void send_to_workspace(args_u *);

key_stroke_t keys[MAX_WORKSPACES * 2 + 2] = {};

static void make_workspace_keys(key_stroke_t *bindings, int *index)
{
	for (int i = 0; i < MAX_WORKSPACES; ++i) {
		bindings[(*index)++] = (key_stroke_t){ XCB_MOD_MASK_4, XK_0 + i, move_to_workspace, { .i = i } };
		bindings[(*index)++] =
			(key_stroke_t){ XCB_MOD_MASK_SHIFT | XCB_MOD_MASK_4, XK_0 + i, send_to_workspace, { .i = i } };
	}
}

static void grab_key_combination(xcb_key_symbols_t *keysyms, xcb_mod_mask_t mask, xcb_keysym_t keysym)
{
	xcb_keycode_t *keycodes = xcb_key_symbols_get_keycode(keysyms, keysym);
	for (xcb_keycode_t *keycode = keycodes; keycode && *keycode != XCB_NO_SYMBOL; keycode++) {
		xcb_grab_key(conn, 0, screen->root, mask, *keycode, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
	}
	free(keycodes);
}

static void quit(args_u *args)
{
	exit(args->i);
}

static void move_to_workspace(args_u *args)
{
	switch_ws(&workspace, args->i);
}

static void send_to_workspace(args_u *args)
{
	send_ws(&workspace, args->i);
}

static void kill_window(args_u *args)
{
	window_kill();
}

void set_key()
{
	int index = 0;
	keys[index++] = (key_stroke_t){ XCB_MOD_MASK_SHIFT | XCB_MOD_MASK_4, XK_l, quit, { .i = 0 } };
	keys[index++] = (key_stroke_t){ XCB_MOD_MASK_SHIFT | XCB_MOD_MASK_4, XK_q, kill_window, {} };
	make_workspace_keys(keys, &index);

	for (size_t i = 0; i < sizeof(keys) / sizeof(key_stroke_t); i++) {
		grab_key_combination(keysyms, keys[i].mod, keys[i].keysym);
	}
	xcb_flush(conn);
}

void handle_key(uint16_t mask, xcb_keycode_t keycode)
{
	for (int i = 0; i < sizeof(keys) / sizeof(key_stroke_t); i++) {
		xcb_keysym_t keysym = xcb_key_symbols_get_keysym(keysyms, keycode, 0);
		if (keys[i].mod == mask && keys[i].keysym == keysym) {
			keys[i].func(&keys[i].args);
			break;
		}
	}
}
