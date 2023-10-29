#ifndef WM_KEY_H
#define WM_KEY_H

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include <X11/keysym.h>

typedef union {
	int i;
} args_u;

typedef struct {
	uint16_t mod;
	xcb_keysym_t keysym;
	void (*func)(args_u *);
	args_u args;
} key_stroke_t;

void handle_key(uint16_t, xcb_keycode_t);
void set_key();

extern xcb_connection_t *conn;
extern xcb_screen_t *screen;
extern xcb_key_symbols_t *keysyms;

#endif
