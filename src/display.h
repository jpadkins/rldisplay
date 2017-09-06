#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

struct display;

// Window management

extern struct display *
display_create(unsigned width, unsigned height, char *title, bool fscreen);

extern void
display_fscreen(struct display *this, bool fscreen);

extern void
display_resize(struct display *this, unsigned width, unsigned height);

extern void
display_rename(struct display *this, const char *title);

extern void
display_vsync(struct display *this, bool enabled);

extern void
display_cursor(struct display *this, bool visible);

extern void
display_framerate(struct display *this, unsigned limit);

extern void
display_cleanup(struct display *this);

extern bool
display_status(struct display *this);

extern void
display_events_flush(struct display *this);

// Rendering

extern void
display_clear(struct display *this);

extern void
display_clear_color(struct display *this, unsigned char rgba[4]);

extern void
display_present(struct display *this);

// Input

enum display_key;

extern bool
display_key_pressed(struct display *this, enum display_key key);

enum display_key {
    DISPLAY_KEY_A,
    DISPLAY_KEY_B,
    DISPLAY_KEY_C,
    DISPLAY_KEY_D,
    DISPLAY_KEY_E,
    DISPLAY_KEY_F,
    DISPLAY_KEY_G,
    DISPLAY_KEY_H,
    DISPLAY_KEY_I,
    DISPLAY_KEY_J,
    DISPLAY_KEY_K,
    DISPLAY_KEY_L,
    DISPLAY_KEY_M,
    DISPLAY_KEY_N,
    DISPLAY_KEY_O,
    DISPLAY_KEY_P,
    DISPLAY_KEY_Q,
    DISPLAY_KEY_R,
    DISPLAY_KEY_S,
    DISPLAY_KEY_T,
    DISPLAY_KEY_U,
    DISPLAY_KEY_V,
    DISPLAY_KEY_W,
    DISPLAY_KEY_X,
    DISPLAY_KEY_Y,
    DISPLAY_KEY_Z,
    DISPLAY_KEY_0,
    DISPLAY_KEY_1,
    DISPLAY_KEY_2,
    DISPLAY_KEY_3,
    DISPLAY_KEY_4,
    DISPLAY_KEY_5,
    DISPLAY_KEY_6,
    DISPLAY_KEY_7,
    DISPLAY_KEY_8,
    DISPLAY_KEY_9,
    DISPLAY_KEY_ESCAPE,
    DISPLAY_KEY_CONTROL,
    DISPLAY_KEY_SHIFT,
    DISPLAY_KEY_ALT,
    DISPLAY_KEY_SYSTEM,
    DISPLAY_KEY_SEMICOLON,
    DISPLAY_KEY_COMMA,
    DISPLAY_KEY_PERIOD,
    DISPLAY_KEY_QUOTE,
    DISPLAY_KEY_SLASH,
    DISPLAY_KEY_TILDE,
    DISPLAY_KEY_SPACE,
    DISPLAY_KEY_ENTER,
    DISPLAY_KEY_BACKSPACE,
    DISPLAY_KEY_TAB,
    DISPLAY_KEY_UP,
    DISPLAY_KEY_DOWN,
    DISPLAY_KEY_LEFT,
    DISPLAY_KEY_RIGHT,

    DISPLAY_KEY_MAXIMUM
};

#endif
