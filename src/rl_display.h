#ifndef RLDISPLAY_H
#define RLDISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
Structs
******************************************************************************/

typedef struct rl_tile rl_tile;
typedef struct rl_tilemap rl_tilemap;
typedef struct rl_display rl_display;
typedef struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a; } rl_color;

/******************************************************************************
Enums
******************************************************************************/

typedef enum {
    RL_TILE_TEXT,
    RL_TILE_EXACT,
    RL_TILE_FLOOR,
    RL_TILE_CENTER
} rl_tiletype;

typedef enum {
    RL_KEY_A,
    RL_KEY_B,
    RL_KEY_C,
    RL_KEY_D,
    RL_KEY_E,
    RL_KEY_F,
    RL_KEY_G,
    RL_KEY_H,
    RL_KEY_I,
    RL_KEY_J,
    RL_KEY_K,
    RL_KEY_L,
    RL_KEY_M,
    RL_KEY_N,
    RL_KEY_O,
    RL_KEY_P,
    RL_KEY_Q,
    RL_KEY_R,
    RL_KEY_S,
    RL_KEY_T,
    RL_KEY_U,
    RL_KEY_V,
    RL_KEY_W,
    RL_KEY_X,
    RL_KEY_Y,
    RL_KEY_Z,
    RL_KEY_0,
    RL_KEY_1,
    RL_KEY_2,
    RL_KEY_3,
    RL_KEY_4,
    RL_KEY_5,
    RL_KEY_6,
    RL_KEY_7,
    RL_KEY_8,
    RL_KEY_9,
    RL_KEY_ESCAPE,
    RL_KEY_CONTROL,
    RL_KEY_SHIFT,
    RL_KEY_ALT,
    RL_KEY_SYSTEM,
    RL_KEY_SEMICOLON,
    RL_KEY_COMMA,
    RL_KEY_PERIOD,
    RL_KEY_QUOTE,
    RL_KEY_SLASH,
    RL_KEY_TILDE,
    RL_KEY_SPACE,
    RL_KEY_ENTER,
    RL_KEY_BACKSPACE,
    RL_KEY_TAB,
    RL_KEY_UP,
    RL_KEY_DOWN,
    RL_KEY_LEFT,
    RL_KEY_RIGHT,
    RL_KEY_MOUSELEFT,
    RL_KEY_MOUSERIGHT,
    RL_KEY_MOUSEMIDDLE,

    RL_KEY_MAXIMUM
} rl_key;

/******************************************************************************
rl_display function declarations
******************************************************************************/

extern rl_display *
rl_display_create(int windoww, int windowh, int framew,
    int frameh, char *title, bool fscreen);

extern void
rl_display_fscreen(rl_display *this, bool fscreen);

extern void
rl_display_resize(rl_display *this, int width, int height);

extern void
rl_display_rename(rl_display *this, const char *title);

extern void
rl_display_vsync(rl_display *this, bool enabled);

extern void
rl_display_cursor(rl_display *this, bool visible);

extern void
rl_display_fps_limit(rl_display *this, int limit);

extern void
rl_display_cleanup(rl_display *this);

extern bool
rl_display_status(rl_display *this);

extern void
rl_display_events_flush(rl_display *this);

extern void
rl_display_clear(rl_display *this);

extern void
rl_display_clear_color(rl_display *this, rl_color color);

extern void
rl_display_draw_tilemap(rl_display *this, rl_tilemap *tmap);

extern void
rl_display_present(rl_display *this);

extern bool
rl_display_key_pressed(rl_display *this, rl_key key);

extern int
rl_display_mouse_x(rl_display *this);

extern int
rl_display_mouse_y(rl_display *this);

extern void
rl_display_mouse(rl_display *this, int *x, int *y);

/******************************************************************************
rl_tile function declarations
******************************************************************************/

extern rl_tile *
rl_tile_create(wchar_t glyph, rl_color fg, rl_color bg, rl_tiletype type,
    float right, float bottom);

extern rl_tile *
rl_tile_default(void);

extern void
rl_tile_glyph(rl_tile *this, wchar_t glyph);

extern void
rl_tile_fg(rl_tile *this, rl_color color);

extern void
rl_tile_bg(rl_tile *this, rl_color color);

extern void
rl_tile_type(rl_tile *this, rl_tiletype type);

extern void
rl_tile_right(rl_tile *this, float right);

extern void
rl_tile_bottom(rl_tile *this, float bottom);

extern void
rl_tile_cleanup(rl_tile *this);

/******************************************************************************
rl_tilemap function declarations
******************************************************************************/

extern rl_tilemap *
rl_tilemap_create(const char *font, int chrsize, int width,
    int height, int offsetx, int offsety);

extern void
rl_tilemap_pos(rl_tilemap *this, float x, float y);

extern void
rl_tilemap_tile(rl_tilemap *this, rl_tile *tile, int x, int y);

extern void
rl_tilemap_wstr_right(rl_tilemap *this, wchar_t *str, rl_color fg, rl_color bg,
    rl_tiletype type, int x, int y);

extern void
rl_tilemap_wstr_down(rl_tilemap *this, wchar_t *str, rl_color fg, rl_color bg,
    rl_tiletype type, int x, int y);

extern void
rl_tilemap_cleanup(rl_tilemap *this);

extern int
rl_tilemap_mouse_x(rl_tilemap *this, rl_display *disp);

extern int
rl_tilemap_mouse_y(rl_tilemap *this, rl_display *disp);

extern void
rl_tilemap_mouse(rl_tilemap *this, rl_display *disp, int *x, int *y);

#ifdef __cplusplus
}
#endif

#endif /* RLDISPLAY_H */
