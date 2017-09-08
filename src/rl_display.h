#ifndef RLDISPLAY_H
#define RLDISPLAY_H

#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>

// RLDisplay

typedef struct _RLDisplay RLDisplay;
typedef struct _RLTileMap RLTileMap;
typedef struct _RLTile RLTile;

typedef enum {
    RLTILE_TEXT,
    RLTILE_EXACT,
    RLTILE_FLOOR,
    RLTILE_CENTER
} RLTileType;

typedef enum {
    RLDISPLAY_KEY_A,
    RLDISPLAY_KEY_B,
    RLDISPLAY_KEY_C,
    RLDISPLAY_KEY_D,
    RLDISPLAY_KEY_E,
    RLDISPLAY_KEY_F,
    RLDISPLAY_KEY_G,
    RLDISPLAY_KEY_H,
    RLDISPLAY_KEY_I,
    RLDISPLAY_KEY_J,
    RLDISPLAY_KEY_K,
    RLDISPLAY_KEY_L,
    RLDISPLAY_KEY_M,
    RLDISPLAY_KEY_N,
    RLDISPLAY_KEY_O,
    RLDISPLAY_KEY_P,
    RLDISPLAY_KEY_Q,
    RLDISPLAY_KEY_R,
    RLDISPLAY_KEY_S,
    RLDISPLAY_KEY_T,
    RLDISPLAY_KEY_U,
    RLDISPLAY_KEY_V,
    RLDISPLAY_KEY_W,
    RLDISPLAY_KEY_X,
    RLDISPLAY_KEY_Y,
    RLDISPLAY_KEY_Z,
    RLDISPLAY_KEY_0,
    RLDISPLAY_KEY_1,
    RLDISPLAY_KEY_2,
    RLDISPLAY_KEY_3,
    RLDISPLAY_KEY_4,
    RLDISPLAY_KEY_5,
    RLDISPLAY_KEY_6,
    RLDISPLAY_KEY_7,
    RLDISPLAY_KEY_8,
    RLDISPLAY_KEY_9,
    RLDISPLAY_KEY_ESCAPE,
    RLDISPLAY_KEY_CONTROL,
    RLDISPLAY_KEY_SHIFT,
    RLDISPLAY_KEY_ALT,
    RLDISPLAY_KEY_SYSTEM,
    RLDISPLAY_KEY_SEMICOLON,
    RLDISPLAY_KEY_COMMA,
    RLDISPLAY_KEY_PERIOD,
    RLDISPLAY_KEY_QUOTE,
    RLDISPLAY_KEY_SLASH,
    RLDISPLAY_KEY_TILDE,
    RLDISPLAY_KEY_SPACE,
    RLDISPLAY_KEY_ENTER,
    RLDISPLAY_KEY_BACKSPACE,
    RLDISPLAY_KEY_TAB,
    RLDISPLAY_KEY_UP,
    RLDISPLAY_KEY_DOWN,
    RLDISPLAY_KEY_LEFT,
    RLDISPLAY_KEY_RIGHT,

    RLDISPLAY_KEY_MAXIMUM
} RLDisplayKey;

extern RLDisplay *
RLDisplay_create(uint32_t width, uint32_t height, char *title, bool fscreen);

extern void
RLDisplay_fscreen(RLDisplay *this, bool fscreen);

extern void
RLDisplay_resize(RLDisplay *this, uint32_t width, uint32_t height);

extern void
RLDisplay_rename(RLDisplay *this, const char *title);

extern void
RLDisplay_vsync(RLDisplay *this, bool enabled);

extern void
RLDisplay_cursor(RLDisplay *this, bool visible);

extern void
RLDisplay_framerate(RLDisplay *this, uint32_t limit);

extern void
RLDisplay_cleanup(RLDisplay *this);

extern bool
RLDisplay_status(RLDisplay *this);

extern void
RLDisplay_events_flush(RLDisplay *this);

extern void
RLDisplay_clear(RLDisplay *this);

extern void
RLDisplay_clear_color(RLDisplay *this, uint8_t rgba[4]);

extern void
RLDisplay_draw_tilemap(RLDisplay *this, RLTileMap *tmap, float x, float y);

extern void
RLDisplay_present(RLDisplay *this);

extern bool
RLDisplay_key_pressed(RLDisplay *this, RLDisplayKey key);

// RLTile

extern RLTile *
RLTile_create(wchar_t glyph, uint8_t fg[4], uint8_t bg[4], RLTileType type,
    float right, float bottom);

extern RLTile *
RLTile_default(void);

extern void
RLTile_set_glyph(RLTile *this, wchar_t glyph);

extern void
RLTile_set_fg(RLTile *this, uint8_t fg[4]);

extern void
RLTile_set_bg(RLTile *this, uint8_t fg[4]);

extern void
RLTile_set_type(RLTile *this, RLTileType type);

extern void
RLTile_set_right(RLTile *this, float right);

extern void
RLTile_set_bottom(RLTile *this, float bottom);

extern void
RLTile_cleanup(RLTile *this);

// RLTileMap 

extern RLTileMap *
RLTileMap_create(const char *font, uint32_t chrsize, uint32_t offsetx,
    uint32_t offsety, uint32_t width, uint32_t height);

extern void
RLTileMap_put_tile(RLTileMap *this, RLTile *tile, uint32_t x, uint32_t y);

extern void
RLTileMap_cleanup(RLTileMap *this);

#endif /* RLDISPLAY_H */
