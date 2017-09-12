#include "rl_display.h"

#include <stdio.h>
#include <stdlib.h>

#include "glad.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define UNUSED(x) (void)x

/******************************************************************************
Struct definitions
******************************************************************************/

/******************************************************************************
Static global variables
******************************************************************************/

/******************************************************************************
Static function declarations
******************************************************************************/

/******************************************************************************
Static function implementations
******************************************************************************/

/******************************************************************************
GLFW callbacks
******************************************************************************/

static void
glfwCallbackError (int err, const char *desc)
{
    UNUSED(err);
    UNUSED(desc);
}

static void
glfwCallbackKey(GLFWwindow *win, int key, int code, int action, int mods)
{
    UNUSED(win);
    UNUSED(key);
    UNUSED(code);
    UNUSED(action);
    UNUSED(mods);
}

static void
glfwCallbackResize(GLFWwindow *win, int width, int height)
{
    UNUSED(win);
    UNUSED(width);
    UNUSED(height);
}

/******************************************************************************
RLDisplay function implementations
******************************************************************************/
/*
extern RLDisplay *
RLDisplay_create(uint32_t winw, uint32_t winh, char *title, bool fscreen,
    uint32_t framew, uint32_t frameh);

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
*/

/******************************************************************************
RLTile function implementations
******************************************************************************/
/*
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
*/

/******************************************************************************
RLTileMap function implementations
******************************************************************************/
/*
extern RLTileMap *
RLTileMap_create(const char *font, uint32_t chrsize, uint32_t offsetx,
    uint32_t offsety, uint32_t width, uint32_t height);

extern void
RLTileMap_put_tile(RLTileMap *this, RLTile *tile, uint32_t x, uint32_t y);

extern void
RLTileMap_cleanup(RLTileMap *this);
*/
