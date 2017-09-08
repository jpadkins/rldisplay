#include "rl_display.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SFML/System.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>

struct _RLDisplay
{
    struct {
        char *title;
        uint32_t width;
        uint32_t height;
        sfColor clear_color;
        sfRenderWindow *handle;
    } window;
};

struct _RLTile {
    float right;
    float bottom;
    wchar_t glyph;
    sfColor fg[4];
    sfColor bg[4];
    RLTileType type;
};

struct _RLTileMap
{
    sfFont *font;
    uint32_t width;
    uint32_t height;
    uint32_t offsetx;
    uint32_t offsety;
    uint32_t chrsize;
    sfVertexArray *fg;
    sfVertexArray *bg;
};

static sfGlyph *glyphs[65536];

static char *
strdup(const char *s);
static void
RLDisplay_resized(RLDisplay *this, uint32_t width, uint32_t height);
static void
RLTileMap_check_glyph(RLTileMap *this, wchar_t glyph);
static void
RLTileMap_gen_tile(RLTileMap *this, RLTile *tile, uint32_t x, uint32_t y);
static void
RLTileMap_gen_fg(RLTileMap *this, uint32_t index, RLTile *tile, uint32_t x,
    uint32_t y, float r, float b, sfIntRect *rect);
static void
RLTileMap_gen_bg(RLTileMap *this, uint32_t index, RLTile *tile, uint32_t x,
    uint32_t y);
static uint32_t
RLTileMap_get_index(RLTileMap *this, uint32_t x, uint32_t y);

static char *
strdup(const char *s)
{
    char *d = NULL;
    size_t len = strlen(s);

    if (!s || !(d = malloc(len + 1)))
        return NULL;

    d[len] = '\0';
    strcpy(d, s);
    return d;
}

static void
RLDisplay_resized(RLDisplay *this, uint32_t width, uint32_t height)
{
    if (!this)
        return;

    this->window.width = width;
    this->window.height = height;
}

RLDisplay *
RLDisplay_create(uint32_t width, uint32_t height, char *title, bool fscreen)
{
    RLDisplay *this = NULL;
    sfVideoMode mode = {width, height, 32};
    sfUint32 style = (fscreen) ? sfFullscreen : sfClose | sfTitlebar;

    if (!(this = malloc(sizeof(RLDisplay))))
        return NULL;

    if (!(this->window.title = strdup(title)))
    {
        free(this);
        return NULL;
    }

    if (!(this->window.handle = sfRenderWindow_create(mode, title, style,
        NULL)))
    {
        free(this->window.title);
        free(this);

        return NULL;
    }

    this->window.clear_color = sfBlack;

    for (size_t i = 0; i < 65536; ++i)
        glyphs[i] = NULL;

    return this;
}

void
RLDisplay_fscreen(RLDisplay *this, bool fscreen)
{
    sfUint32 style = (fscreen) ? sfFullscreen : sfDefaultStyle;
    sfVideoMode mode = {this->window.width, this->window.height, 32};

    if (!this || !this->window.handle)
        return;

    sfRenderWindow_destroy(this->window.handle);

    this->window.handle = sfRenderWindow_create(mode, this->window.title,
        style, NULL);
}

void
RLDisplay_resize(RLDisplay *this, uint32_t width, uint32_t height)
{
    sfVector2u size = {width, height};

    if (!this || !this->window.handle)
        return;

    this->window.width = width;
    this->window.height = height;
    sfRenderWindow_setSize(this->window.handle, size);
}

void
RLDisplay_rename(RLDisplay *this, const char *title)
{
    if (!this || !this->window.title)
        return;

    free(this->window.title);

    this->window.title = strdup(title);

    sfRenderWindow_setTitle(this->window.handle, title);
}

void
RLDisplay_vsync(RLDisplay *this, bool enabled)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setVerticalSyncEnabled(this->window.handle, enabled);
}

void
RLDisplay_cursor(RLDisplay *this, bool visible)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setMouseCursorVisible(this->window.handle, visible);
}

void
RLDisplay_framerate(RLDisplay *this, uint32_t limit)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setFramerateLimit(this->window.handle, limit);
}

void
RLDisplay_cleanup(RLDisplay *this)
{
    if (!this)
        return;

    for (size_t i = 0; i < 65536; ++i)
        if (glyphs[i]) free(glyphs[i]);

    sfRenderWindow_destroy(this->window.handle);
    free(this->window.title);
    free(this);
}

bool
RLDisplay_status(RLDisplay *this)
{
    if (!this || !this->window.handle)
        return false;

    return sfRenderWindow_isOpen(this->window.handle);
}

void
RLDisplay_events_flush(RLDisplay *this)
{
    static sfEvent evt;

    if (!this || !this->window.handle)
        return;

    while (sfRenderWindow_pollEvent(this->window.handle, &evt))
    {
        switch (evt.type)
        {
            case sfEvtClosed:
                sfRenderWindow_close(this->window.handle);
                break;
            case sfEvtResized:
                RLDisplay_resized(this, evt.size.width, evt.size.height);
                break;
            default:
                break;
        }
    }
}

void
RLDisplay_clear(RLDisplay *this)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_clear(this->window.handle, this->window.clear_color);
}

void
RLDisplay_clear_color(RLDisplay *this, uint8_t rgba[4])
{
    if (!this)
        return;

    this->window.clear_color = (sfColor){rgba[0], rgba[1], rgba[2], rgba[3]};
}

void
RLDisplay_draw_tilemap(RLDisplay *this, RLTileMap *tmap, float x, float y)
{
    static sfRenderStates states;

    if (!this || !this->window.handle || !tmap)
        return;

    states.shader = NULL;
    states.blendMode = sfBlendAlpha;
    states.transform = sfTransform_Identity;
    sfTransform_translate(&states.transform, x, y);
    states.texture = sfFont_getTexture(tmap->font, tmap->chrsize);

    sfRenderWindow_drawVertexArray(this->window.handle, tmap->bg, &states);
    sfRenderWindow_drawVertexArray(this->window.handle, tmap->fg, &states);
}

void
RLDisplay_present(RLDisplay *this)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_display(this->window.handle);
}

bool
RLDisplay_key_pressed(RLDisplay *this, RLDisplayKey key)
{
    if (!this || key < 0 || key >= RLDISPLAY_KEY_MAXIMUM)
        return false;

    // *this is unused in the SFML renderer implementation, but the interface
    // requires it to be passed for compatibility with other backends
    (void)this;

    switch (key)
    {
    case RLDISPLAY_KEY_A:
        return sfKeyboard_isKeyPressed(sfKeyA);
    case RLDISPLAY_KEY_B:
        return sfKeyboard_isKeyPressed(sfKeyB);
    case RLDISPLAY_KEY_C:
        return sfKeyboard_isKeyPressed(sfKeyC);
    case RLDISPLAY_KEY_D:
        return sfKeyboard_isKeyPressed(sfKeyD);
    case RLDISPLAY_KEY_E:
        return sfKeyboard_isKeyPressed(sfKeyE);
    case RLDISPLAY_KEY_F:
        return sfKeyboard_isKeyPressed(sfKeyF);
    case RLDISPLAY_KEY_G:
        return sfKeyboard_isKeyPressed(sfKeyG);
    case RLDISPLAY_KEY_H:
        return sfKeyboard_isKeyPressed(sfKeyH);
    case RLDISPLAY_KEY_I:
        return sfKeyboard_isKeyPressed(sfKeyI);
    case RLDISPLAY_KEY_J:
        return sfKeyboard_isKeyPressed(sfKeyJ);
    case RLDISPLAY_KEY_K:
        return sfKeyboard_isKeyPressed(sfKeyK);
    case RLDISPLAY_KEY_L:
        return sfKeyboard_isKeyPressed(sfKeyL);
    case RLDISPLAY_KEY_M:
        return sfKeyboard_isKeyPressed(sfKeyM);
    case RLDISPLAY_KEY_N:
        return sfKeyboard_isKeyPressed(sfKeyN);
    case RLDISPLAY_KEY_O:
        return sfKeyboard_isKeyPressed(sfKeyO);
    case RLDISPLAY_KEY_P:
        return sfKeyboard_isKeyPressed(sfKeyP);
    case RLDISPLAY_KEY_Q:
        return sfKeyboard_isKeyPressed(sfKeyQ);
    case RLDISPLAY_KEY_R:
        return sfKeyboard_isKeyPressed(sfKeyR);
    case RLDISPLAY_KEY_S:
        return sfKeyboard_isKeyPressed(sfKeyS);
    case RLDISPLAY_KEY_T:
        return sfKeyboard_isKeyPressed(sfKeyT);
    case RLDISPLAY_KEY_U:
        return sfKeyboard_isKeyPressed(sfKeyU);
    case RLDISPLAY_KEY_V:
        return sfKeyboard_isKeyPressed(sfKeyV);
    case RLDISPLAY_KEY_W:
        return sfKeyboard_isKeyPressed(sfKeyW);
    case RLDISPLAY_KEY_X:
        return sfKeyboard_isKeyPressed(sfKeyX);
    case RLDISPLAY_KEY_Y:
        return sfKeyboard_isKeyPressed(sfKeyY);
    case RLDISPLAY_KEY_Z:
        return sfKeyboard_isKeyPressed(sfKeyZ);
    case RLDISPLAY_KEY_0:
        return sfKeyboard_isKeyPressed(sfKeyNum0) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad0);
    case RLDISPLAY_KEY_1:
        return sfKeyboard_isKeyPressed(sfKeyNum1) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad1);
    case RLDISPLAY_KEY_2:
        return sfKeyboard_isKeyPressed(sfKeyNum2) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad2);
    case RLDISPLAY_KEY_3:
        return sfKeyboard_isKeyPressed(sfKeyNum3) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad3);
    case RLDISPLAY_KEY_4:
        return sfKeyboard_isKeyPressed(sfKeyNum4) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad4);
    case RLDISPLAY_KEY_5:
        return sfKeyboard_isKeyPressed(sfKeyNum5) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad5);
    case RLDISPLAY_KEY_6:
        return sfKeyboard_isKeyPressed(sfKeyNum6) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad6);
    case RLDISPLAY_KEY_7:
        return sfKeyboard_isKeyPressed(sfKeyNum7) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad7);
    case RLDISPLAY_KEY_8:
        return sfKeyboard_isKeyPressed(sfKeyNum8) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad8);
    case RLDISPLAY_KEY_9:
        return sfKeyboard_isKeyPressed(sfKeyNum9) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad9);
    case RLDISPLAY_KEY_ESCAPE:
        return sfKeyboard_isKeyPressed(sfKeyEscape);
    case RLDISPLAY_KEY_CONTROL:
        return sfKeyboard_isKeyPressed(sfKeyLControl) ||
            sfKeyboard_isKeyPressed(sfKeyRControl);
    case RLDISPLAY_KEY_SHIFT:
        return sfKeyboard_isKeyPressed(sfKeyLShift) ||
            sfKeyboard_isKeyPressed(sfKeyRShift);
    case RLDISPLAY_KEY_ALT:
        return sfKeyboard_isKeyPressed(sfKeyLAlt) ||
            sfKeyboard_isKeyPressed(sfKeyRAlt);
    case RLDISPLAY_KEY_SYSTEM:
        return sfKeyboard_isKeyPressed(sfKeyLSystem) ||
            sfKeyboard_isKeyPressed(sfKeyRSystem);
    case RLDISPLAY_KEY_SEMICOLON:
        return sfKeyboard_isKeyPressed(sfKeySemiColon);
    case RLDISPLAY_KEY_COMMA:
        return sfKeyboard_isKeyPressed(sfKeyComma);
    case RLDISPLAY_KEY_PERIOD:
        return sfKeyboard_isKeyPressed(sfKeyPeriod);
    case RLDISPLAY_KEY_QUOTE:
        return sfKeyboard_isKeyPressed(sfKeyQuote);
    case RLDISPLAY_KEY_SLASH:
        return sfKeyboard_isKeyPressed(sfKeySlash);
    case RLDISPLAY_KEY_TILDE:
        return sfKeyboard_isKeyPressed(sfKeyTilde);
    case RLDISPLAY_KEY_SPACE:
        return sfKeyboard_isKeyPressed(sfKeySpace);
    case RLDISPLAY_KEY_ENTER:
        return sfKeyboard_isKeyPressed(sfKeyReturn);
    case RLDISPLAY_KEY_BACKSPACE:
        return sfKeyboard_isKeyPressed(sfKeyBack);
    case RLDISPLAY_KEY_TAB:
        return sfKeyboard_isKeyPressed(sfKeyTab);
    case RLDISPLAY_KEY_UP:
        return sfKeyboard_isKeyPressed(sfKeyUp);
    case RLDISPLAY_KEY_DOWN:
        return sfKeyboard_isKeyPressed(sfKeyDown);
    case RLDISPLAY_KEY_LEFT:
        return sfKeyboard_isKeyPressed(sfKeyLeft);
    case RLDISPLAY_KEY_RIGHT:
        return sfKeyboard_isKeyPressed(sfKeyRight);
    default:
        return false;
    }
}

RLTile *
RLTile_create(wchar_t glyph, uint8_t fg[4], uint8_t bg[4],
    RLTileType type, float right, float bottom)
{
    RLTile *this = NULL;

    if (!(this = malloc(sizeof(RLTile))))
        return NULL;

    this->glyph = glyph;
    
    for (int i = 0; i < 4; ++i)
    {
        this->fg[i] = sfColor_fromRGBA(fg[0], fg[1], fg[2], fg[3]);
        this->bg[i] = sfColor_fromRGBA(bg[0], bg[1], bg[2], bg[3]);
    }

    this->type = type;
    this->right = right;
    this->bottom = bottom;

    return this;
}

RLTile *
RLTile_default(void)
{
    uint8_t fg[4] = {255, 0, 0, 255};
    uint8_t bg[4] = {0, 0, 255, 255};

    return RLTile_create(L'?', fg, bg, RLTILE_CENTER, 0.0f, 0.0f);
}

void
RLTile_set_glyph(RLTile *this, wchar_t glyph)
{
    if (!this)
        return;

    this->glyph = glyph;
}

void
RLTile_set_fg(RLTile *this, uint8_t fg[4])
{
    if (!this || !fg)
        return;

    for (size_t i = 0; i < 4; ++i)
        this->fg[i] = sfColor_fromRGBA(fg[0], fg[1], fg[2], fg[3]);
}

void
RLTile_set_bg(RLTile *this, uint8_t bg[4])
{
    if (!this || !bg)
        return;

    for (size_t i = 0; i < 4; ++i)
        this->bg[i] = sfColor_fromRGBA(bg[0], bg[1], bg[2], bg[3]);
}

void
RLTile_set_type(RLTile *this, RLTileType type)
{
    if (!this)
        return;

    this->type = type;
}

void
RLTile_set_right(RLTile *this, float right)
{
    if (!this)
        return;

    this->right = right;
}

void
RLTile_set_bottom(RLTile *this, float bottom)
{
    if (!this)
        return;

    this->bottom = bottom;
}

void
RLTile_cleanup(RLTile *this)
{
    if (!this)
        return;

    free(this);
}

static void
RLTileMap_check_glyph(RLTileMap *this, wchar_t glyph)
{
    if (!this)
        return;

    if (!glyphs[(size_t)glyph] &&
        (glyphs[(size_t)glyph] = malloc(sizeof(sfGlyph))))
    {
        *glyphs[(size_t)glyph] = sfFont_getGlyph(this->font, (uint32_t)glyph,
            this->chrsize, false, 0.0f);
    }
}

static void
RLTileMap_gen_tile(RLTileMap *this, RLTile *tile, uint32_t x, uint32_t y)
{
    uint32_t index;
    sfIntRect rect;
    sfFloatRect bounds;
    float right = 0.0f, bottom = 0.0f;

    if (!this || !tile)
        return;

    RLTileMap_check_glyph(this, tile->glyph);
    rect = glyphs[(size_t)(tile->glyph)]->textureRect;

    switch (tile->type)
    {
    case RLTILE_TEXT:

        bounds = glyphs[(size_t)(tile->glyph)]->bounds;

        right = bounds.left;
        bottom = (float)(this->offsety) + bounds.top;
        break;

    case RLTILE_EXACT:

        right = tile->right;
        bottom = tile->bottom;
        break;

    case RLTILE_FLOOR:

        right = (float)((int)(((float)(this->offsetx) -
            (float)(rect.width))/2.0f));
        bottom = (float)((int)((float)(this->offsety) -
            (float)(rect.height)));
        break;

    case RLTILE_CENTER:

        right = (float)((int)(((float)(this->offsetx) -
            (float)(rect.width))/2.0f));
        bottom = (float)((int)(((float)(this->offsety) -
            (float)(rect.height))/2.0f));
        break;

    }

    index = RLTileMap_get_index(this, x, y);

    RLTileMap_gen_fg(this, index, tile, x, y, right, bottom, &rect);
    RLTileMap_gen_bg(this, index, tile, x, y);
}

static void
RLTileMap_gen_fg(RLTileMap *this, uint32_t index, RLTile *tile, uint32_t x,
    uint32_t y, float r, float b, sfIntRect *rect)
{
    if (!this || !tile || !rect)
        return;

    index *= 4;

    sfVertexArray_getVertex(this->fg, index)->position = (sfVector2f){
        (float)x + r,
        (float)y * (float)(this->offsety) + b
    };

    sfVertexArray_getVertex(this->fg, index + 1)->position = (sfVector2f){
        (float)x * (float)(this->offsetx) + (float)(rect->width) + r,
        (float)y * (float)(this->offsety) + b
    };

    sfVertexArray_getVertex(this->fg, index + 2)->position = (sfVector2f){
        (float)x * (float)(this->offsetx) + (float)(rect->width) + r,
        (float)y * (float)(this->offsety) + (float)(rect->height) + b
    };

    sfVertexArray_getVertex(this->fg, index + 3)->position = (sfVector2f){
        (float)x * (float)(this->offsetx) + r,
        (float)y * (float)(this->offsety) + (float)(rect->height) + b
    };

    sfVertexArray_getVertex(this->fg, index)->texCoords = (sfVector2f){
        (float)(rect->left),
        (float)(rect->top)
    };

    sfVertexArray_getVertex(this->fg, index + 1)->texCoords = (sfVector2f){
        (float)(rect->left) + (float)(rect->width),
        (float)(rect->top)
    };

    sfVertexArray_getVertex(this->fg, index + 2)->texCoords = (sfVector2f){
        (float)(rect->left) + (float)(rect->width),
        (float)(rect->top) + (float)(rect->height)
    };

    sfVertexArray_getVertex(this->fg, index + 3)->texCoords = (sfVector2f){
        (float)(rect->left),
        (float)(rect->top) + (float)(rect->height)
    };

    sfVertexArray_getVertex(this->fg, index)->color = tile->fg[0];
    sfVertexArray_getVertex(this->fg, index + 1)->color = tile->fg[1];
    sfVertexArray_getVertex(this->fg, index + 2)->color = tile->fg[2];
    sfVertexArray_getVertex(this->fg, index + 3)->color = tile->fg[3];
}

static void
RLTileMap_gen_bg(RLTileMap *this, uint32_t index, RLTile *tile, uint32_t x,
    uint32_t y)
{
    if (!this || !tile)
        return;

    index *= 4;

    sfVertexArray_getVertex(this->bg, index)->position = (sfVector2f){
        (float)x * (float)(this->offsetx),
        (float)y * (float)(this->offsety)
    };

    sfVertexArray_getVertex(this->bg, index + 1)->position = (sfVector2f){
        (float)x * (float)(this->offsetx) + (float)(this->offsetx),
        (float)y * (float)(this->offsety)
    };

    sfVertexArray_getVertex(this->bg, index + 2)->position = (sfVector2f){
        (float)x * (float)(this->offsetx) + (float)(this->offsetx),
        (float)y * (float)(this->offsety) + (float)(this->offsety)
    };

    sfVertexArray_getVertex(this->bg, index + 3)->position = (sfVector2f){
        (float)x * (float)(this->offsetx),
        (float)y * (float)(this->offsety) + (float)(this->offsety)
    };

    sfVertexArray_getVertex(this->bg, index)->color = tile->bg[0];
    sfVertexArray_getVertex(this->bg, index + 1)->color = tile->bg[1];
    sfVertexArray_getVertex(this->bg, index + 2)->color = tile->bg[2];
    sfVertexArray_getVertex(this->bg, index + 3)->color = tile->bg[3];
}

static uint32_t
RLTileMap_get_index(RLTileMap *this, uint32_t x, uint32_t y)
{
    if (!this)
        return 0;

    return (y * this->width) + x;
}

RLTileMap *
RLTileMap_create(const char *font, uint32_t chrsize, uint32_t offsetx,
    uint32_t offsety, uint32_t width, uint32_t height)
{
    RLTileMap *this = NULL;

    if (!(this = malloc(sizeof(RLTileMap))))
        return NULL;

    if (!(this->font = sfFont_createFromFile(font)))
    {
        free(this);
        return NULL;
    }

    if (!(this->fg = sfVertexArray_create()))
    {
        sfFont_destroy(this->font);
        free(this);

        return NULL;
    }

    sfVertexArray_resize(this->fg, width * height * 4);
    sfVertexArray_setPrimitiveType(this->fg, sfQuads);

    if (!(this->bg = sfVertexArray_create()))
    {
        sfVertexArray_destroy(this->fg);
        sfFont_destroy(this->font);
        free(this);

        return NULL;
    }

    sfVertexArray_resize(this->bg, width * height * 4);
    sfVertexArray_setPrimitiveType(this->bg, sfQuads);

    this->width = width;
    this->height = height;
    this->offsetx = offsetx;
    this->offsety = offsety;
    this->chrsize = chrsize;

    return this;
}

void
RLTileMap_put_tile(RLTileMap *this, RLTile *tile, uint32_t x, uint32_t y)
{
    if (!this || !tile)
        return;

    RLTileMap_gen_tile(this, tile, x, y);
}

void
RLTileMap_cleanup(RLTileMap *this)
{
    if (!this)
        return;

    sfFont_destroy(this->font);
    sfVertexArray_destroy(this->fg);
    sfVertexArray_destroy(this->bg);

    free(this);
}
