#include "rl_display.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SFML/System.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>

#define UNUSED(x) (void)x

/******************************************************************************
Struct definitions
******************************************************************************/

struct rl_tile {
    float right;
    float bottom;
    wchar_t glyph;
    sfColor fg[4];
    sfColor bg[4];
    rl_tiletype type;
};

struct rl_tilemap
{
    int width;
    int height;
    float posx;
    float posy;
    int chrsize;
    int offsetx;
    int offsety;
    sfFont *font;
    sfVertexArray *fg;
    sfVertexArray *bg;
    sfGlyph *glyphs[65536];
};

struct rl_display
{
    struct {
        int width;
        int height;
        char *title;
        bool fscreen;
        sfRenderWindow *handle;
    } window;
    
    struct {
        int width;
        int height;
        sfVector2f scale;
        sfColor clear_color;
        sfRenderTexture *handle;
    } frame;
};

/******************************************************************************
Static global variables
******************************************************************************/

/* TODO: Tilemaps with the same chrsize should share their *glyphs arrays */

/* static sfGlyph glyphs[65536]; */

/******************************************************************************
Static function declarations
******************************************************************************/

/* misc */
static char *
strdup(const char *s);

/* rl_display */
static void
rl_display_update_scale(rl_display *this);

static void
rl_display_resized(rl_display *this, int width, int height);

/* rl_tilemap */
static void
rl_tilemap_check_glyph(rl_tilemap *this, wchar_t glyph);

static int
rl_tilemap_get_index(rl_tilemap *this, int x, int y);

static void
rl_tilemap_gen_tile(rl_tilemap *this, rl_tile *tile, int x, int y);

static void
rl_tilemap_gen_bg(rl_tilemap *this, int index, rl_tile *tile, int x,
    int y);

static void
rl_tilemap_gen_fg(rl_tilemap *this, int index, rl_tile *tile, int x,
    int y, float r, float b, sfIntRect *rect);

/******************************************************************************
Misc static function implementations
******************************************************************************/

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

/******************************************************************************
rl_display function implementations
******************************************************************************/

static void
rl_display_resized(rl_display *this, int width, int height)
{
    if (!this)
        return;

    this->window.width = width;
    this->window.height = height;

    rl_display_update_scale(this);
}

static void
rl_display_update_scale(rl_display *this)
{
    if (!this)
        return;

    this->frame.scale = (sfVector2f){
        (float)this->window.width / (float)this->frame.width,
        (float)this->window.height / (float)this->frame.height
    };
}

rl_display *
rl_display_create(int width, int height, int fwidth, int fheight, char *title,
    bool fscreen)
{
    rl_display *this = NULL;
    sfVideoMode mode = {(unsigned)width, (unsigned)height, 32};
    sfUint32 style = (fscreen) ? sfFullscreen : sfClose | sfTitlebar;

    /* TODO: Sanity check for size parameters? */

    if (!(this = malloc(sizeof(rl_display))))
        goto error;

    if (!(this->window.title = strdup(title)))
        goto error;

    if (!(this->window.handle = sfRenderWindow_create(mode, title, style,
        NULL)))
        goto error;

    this->window.width = width;
    this->window.height = height;
    this->window.fscreen = fscreen;

    if (!(this->frame.handle = sfRenderTexture_create((unsigned)fwidth,
        (unsigned)fheight, false)))
        goto error;

    sfRenderWindow_setActive(this->window.handle, true);

    this->frame.width = fwidth;
    this->frame.height = fheight;
    this->frame.clear_color = sfBlack;

    rl_display_update_scale(this);

    return this;

error:

    rl_display_cleanup(this);
    return NULL;
}

void
rl_display_fscreen(rl_display *this, bool fscreen)
{
    sfUint32 style = (fscreen) ? sfFullscreen : sfClose | sfTitlebar;
    sfVideoMode mode = {(unsigned)this->window.width,
        (unsigned)this->window.height, 32};

    if (!this || !this->window.handle)
        return;

    this->window.fscreen = fscreen;

    sfRenderWindow_destroy(this->window.handle);
    this->window.handle = sfRenderWindow_create(mode, this->window.title,
        style, NULL);
}

void
rl_display_resize(rl_display *this, int width, int height)
{
    sfVideoMode mode = {(unsigned)width, (unsigned)height, 32};
    sfUint32 style = (this->window.fscreen) ? sfFullscreen
        : sfClose | sfTitlebar;

    if (!this || !this->window.handle)
        return;

    this->window.width = width;
    this->window.height = height;
    rl_display_update_scale(this);

    sfRenderWindow_destroy(this->window.handle);
    this->window.handle = sfRenderWindow_create(mode, this->window.title,
        style, NULL);
}

void
rl_display_rename(rl_display *this, const char *title)
{
    if (!this || !this->window.title)
        return;

    free(this->window.title);

    this->window.title = strdup(title);

    sfRenderWindow_setTitle(this->window.handle, title);
}

void
rl_display_vsync(rl_display *this, bool enabled)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setVerticalSyncEnabled(this->window.handle, enabled);
}

void
rl_display_cursor(rl_display *this, bool visible)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setMouseCursorVisible(this->window.handle, visible);
}

void
rl_display_fps_limit(rl_display *this, int limit)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setFramerateLimit(this->window.handle, (unsigned)limit);
}

void
rl_display_cleanup(rl_display *this)
{
    if (!this)
        return;

    if (this->frame.handle)
        sfRenderTexture_destroy(this->frame.handle);

    if (this->window.handle)
        sfRenderWindow_destroy(this->window.handle);

    if (this->window.title)
        free(this->window.title);

    if (this)
        free(this);
}

bool
rl_display_status(rl_display *this)
{
    if (!this || !this->window.handle)
        return false;

    return sfRenderWindow_isOpen(this->window.handle);
}

void
rl_display_events_flush(rl_display *this)
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
                rl_display_resized(this, (int)evt.size.width,
                    (int)evt.size.height);
                break;
            default:
                break;
        }
    }
}

void
rl_display_clear(rl_display *this)
{
    if (!this || !this->window.handle)
        return;

    sfRenderTexture_clear(this->frame.handle, this->frame.clear_color);
}

void
rl_display_clear_color(rl_display *this, rl_color color)
{
    if (!this)
        return;

    this->frame.clear_color = (sfColor){color.r, color.g, color.b, color.a};
}

void
rl_display_draw_tilemap(rl_display *this, rl_tilemap *tmap)
{
    sfRenderStates states;

    if (!this || !this->window.handle || !tmap)
        return;

    states.shader = NULL;
    states.blendMode = sfBlendAlpha;
    states.transform = sfTransform_Identity;
    sfTransform_translate(&states.transform, tmap->posx, tmap->posy);
    states.texture = sfFont_getTexture(tmap->font, (unsigned)tmap->chrsize);

    sfRenderTexture_drawVertexArray(this->frame.handle, tmap->bg, &states);
    sfRenderTexture_drawVertexArray(this->frame.handle, tmap->fg, &states);
}

void
rl_display_present(rl_display *this)
{
    sfSprite *sprite = NULL;

    if (!this || !this->window.handle || !this->frame.handle
        || !(sprite = sfSprite_create()))
        return;

    sfRenderTexture_display(this->frame.handle);

    sfSprite_setScale(sprite, this->frame.scale);
    sfSprite_setTexture(sprite,
        sfRenderTexture_getTexture(this->frame.handle), false);

    sfRenderWindow_drawSprite(this->window.handle, sprite, NULL);
    sfRenderWindow_display(this->window.handle);
    sfSprite_destroy(sprite);
}

bool
rl_display_key_pressed(rl_display *this, rl_key key)
{
    if (!this || key < 0 || key >= RL_KEY_MAXIMUM)
        return false;

    /* *this is unused in the SFML renderer implementation, but the interface
       requires it to be passed for compatibility with other backends */
    UNUSED(this);

    switch (key)
    {
    case RL_KEY_A:
        return sfKeyboard_isKeyPressed(sfKeyA);
    case RL_KEY_B:
        return sfKeyboard_isKeyPressed(sfKeyB);
    case RL_KEY_C:
        return sfKeyboard_isKeyPressed(sfKeyC);
    case RL_KEY_D:
        return sfKeyboard_isKeyPressed(sfKeyD);
    case RL_KEY_E:
        return sfKeyboard_isKeyPressed(sfKeyE);
    case RL_KEY_F:
        return sfKeyboard_isKeyPressed(sfKeyF);
    case RL_KEY_G:
        return sfKeyboard_isKeyPressed(sfKeyG);
    case RL_KEY_H:
        return sfKeyboard_isKeyPressed(sfKeyH);
    case RL_KEY_I:
        return sfKeyboard_isKeyPressed(sfKeyI);
    case RL_KEY_J:
        return sfKeyboard_isKeyPressed(sfKeyJ);
    case RL_KEY_K:
        return sfKeyboard_isKeyPressed(sfKeyK);
    case RL_KEY_L:
        return sfKeyboard_isKeyPressed(sfKeyL);
    case RL_KEY_M:
        return sfKeyboard_isKeyPressed(sfKeyM);
    case RL_KEY_N:
        return sfKeyboard_isKeyPressed(sfKeyN);
    case RL_KEY_O:
        return sfKeyboard_isKeyPressed(sfKeyO);
    case RL_KEY_P:
        return sfKeyboard_isKeyPressed(sfKeyP);
    case RL_KEY_Q:
        return sfKeyboard_isKeyPressed(sfKeyQ);
    case RL_KEY_R:
        return sfKeyboard_isKeyPressed(sfKeyR);
    case RL_KEY_S:
        return sfKeyboard_isKeyPressed(sfKeyS);
    case RL_KEY_T:
        return sfKeyboard_isKeyPressed(sfKeyT);
    case RL_KEY_U:
        return sfKeyboard_isKeyPressed(sfKeyU);
    case RL_KEY_V:
        return sfKeyboard_isKeyPressed(sfKeyV);
    case RL_KEY_W:
        return sfKeyboard_isKeyPressed(sfKeyW);
    case RL_KEY_X:
        return sfKeyboard_isKeyPressed(sfKeyX);
    case RL_KEY_Y:
        return sfKeyboard_isKeyPressed(sfKeyY);
    case RL_KEY_Z:
        return sfKeyboard_isKeyPressed(sfKeyZ);
    case RL_KEY_0:
        return sfKeyboard_isKeyPressed(sfKeyNum0) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad0);
    case RL_KEY_1:
        return sfKeyboard_isKeyPressed(sfKeyNum1) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad1);
    case RL_KEY_2:
        return sfKeyboard_isKeyPressed(sfKeyNum2) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad2);
    case RL_KEY_3:
        return sfKeyboard_isKeyPressed(sfKeyNum3) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad3);
    case RL_KEY_4:
        return sfKeyboard_isKeyPressed(sfKeyNum4) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad4);
    case RL_KEY_5:
        return sfKeyboard_isKeyPressed(sfKeyNum5) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad5);
    case RL_KEY_6:
        return sfKeyboard_isKeyPressed(sfKeyNum6) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad6);
    case RL_KEY_7:
        return sfKeyboard_isKeyPressed(sfKeyNum7) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad7);
    case RL_KEY_8:
        return sfKeyboard_isKeyPressed(sfKeyNum8) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad8);
    case RL_KEY_9:
        return sfKeyboard_isKeyPressed(sfKeyNum9) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad9);
    case RL_KEY_ESCAPE:
        return sfKeyboard_isKeyPressed(sfKeyEscape);
    case RL_KEY_CONTROL:
        return sfKeyboard_isKeyPressed(sfKeyLControl) ||
            sfKeyboard_isKeyPressed(sfKeyRControl);
    case RL_KEY_SHIFT:
        return sfKeyboard_isKeyPressed(sfKeyLShift) ||
            sfKeyboard_isKeyPressed(sfKeyRShift);
    case RL_KEY_ALT:
        return sfKeyboard_isKeyPressed(sfKeyLAlt) ||
            sfKeyboard_isKeyPressed(sfKeyRAlt);
    case RL_KEY_SYSTEM:
        return sfKeyboard_isKeyPressed(sfKeyLSystem) ||
            sfKeyboard_isKeyPressed(sfKeyRSystem);
    case RL_KEY_SEMICOLON:
        return sfKeyboard_isKeyPressed(sfKeySemiColon);
    case RL_KEY_COMMA:
        return sfKeyboard_isKeyPressed(sfKeyComma);
    case RL_KEY_PERIOD:
        return sfKeyboard_isKeyPressed(sfKeyPeriod);
    case RL_KEY_QUOTE:
        return sfKeyboard_isKeyPressed(sfKeyQuote);
    case RL_KEY_SLASH:
        return sfKeyboard_isKeyPressed(sfKeySlash);
    case RL_KEY_TILDE:
        return sfKeyboard_isKeyPressed(sfKeyTilde);
    case RL_KEY_SPACE:
        return sfKeyboard_isKeyPressed(sfKeySpace);
    case RL_KEY_ENTER:
        return sfKeyboard_isKeyPressed(sfKeyReturn);
    case RL_KEY_BACKSPACE:
        return sfKeyboard_isKeyPressed(sfKeyBack);
    case RL_KEY_TAB:
        return sfKeyboard_isKeyPressed(sfKeyTab);
    case RL_KEY_UP:
        return sfKeyboard_isKeyPressed(sfKeyUp);
    case RL_KEY_DOWN:
        return sfKeyboard_isKeyPressed(sfKeyDown);
    case RL_KEY_LEFT:
        return sfKeyboard_isKeyPressed(sfKeyLeft);
    case RL_KEY_RIGHT:
        return sfKeyboard_isKeyPressed(sfKeyRight);
    case RL_KEY_MOUSELEFT:
        return sfMouse_isButtonPressed(sfMouseLeft);
    case RL_KEY_MOUSERIGHT:
        return sfMouse_isButtonPressed(sfMouseRight);
    case RL_KEY_MOUSEMIDDLE:
        return sfMouse_isButtonPressed(sfMouseMiddle);
    default:
        return false;
    }
}

int
rl_display_mouse_x(rl_display *this)
{
    int x;

    if (!this || !this->window.handle)
        return 0;

    x = sfMouse_getPositionRenderWindow(this->window.handle).x;

    if (x < 0 || x > this->window.width)
        return x;

    return (int)((float)x / this->frame.scale.x);
}

int
rl_display_mouse_y(rl_display *this)
{
    int y;

    if (!this || !this->window.handle)
        return 0;

    y = sfMouse_getPositionRenderWindow(this->window.handle).y;

    if (y < 0 || y > this->window.height)
        return y;

    return (int)((float)y / this->frame.scale.y);
}

void
rl_display_mouse(rl_display *this, int *x, int *y)
{
    sfVector2i mouse;

    if (!this || !this->window.handle || !x || !y)
        return;

    mouse = sfMouse_getPositionRenderWindow(this->window.handle);

    if (mouse.x < 0 || mouse.x > this->window.width)
        *x = mouse.x;
    else
        *x = (int)((float)mouse.x / this->frame.scale.x);

    if (mouse.y < 0 || mouse.y > this->window.height)
        *y = mouse.y;
    else
        *y = (int)((float)mouse.y / this->frame.scale.y);
}

/******************************************************************************
rl_tile function implementations
******************************************************************************/

rl_tile *
rl_tile_create(wchar_t glyph, rl_color fg, rl_color bg, rl_tiletype type,
    float right, float bottom)
{
    rl_tile *this = NULL;

    if (!(this = malloc(sizeof(rl_tile))))
        return NULL;

    this->glyph = glyph;
    
    for (int i = 0; i < 4; ++i)
    {
        this->fg[i] = (sfColor){fg.r, fg.g, fg.b, fg.a};
        this->bg[i] = (sfColor){bg.r, bg.g, bg.b, bg.a};
    }

    this->type = type;
    this->right = right;
    this->bottom = bottom;

    return this;
}

rl_tile *
rl_tile_default(void)
{
    rl_color fg = {255, 0, 0, 255};
    rl_color bg = {0, 0, 255, 255};

    return rl_tile_create(L'?', fg, bg, RL_TILE_CENTER, 0.0f, 0.0f);
}

void
rl_tile_glyph(rl_tile *this, wchar_t glyph)
{
    if (!this)
        return;

    this->glyph = glyph;
}

void
rl_tile_fg(rl_tile *this, rl_color color)
{
    if (!this)
        return;

    for (size_t i = 0; i < 4; ++i)
        this->fg[i] = (sfColor){color.r, color.g, color.b, color.a};
}

void
rl_tile_bg(rl_tile *this, rl_color color)
{
    if (!this)
        return;

    for (size_t i = 0; i < 4; ++i)
        this->bg[i] = (sfColor){color.r, color.g, color.b, color.a};
}

void
rl_tile_type(rl_tile *this, rl_tiletype type)
{
    if (!this)
        return;

    this->type = type;
}

void
rl_tile_right(rl_tile *this, float right)
{
    if (!this)
        return;

    this->right = right;
}

void
rl_tile_bottom(rl_tile *this, float bottom)
{
    if (!this)
        return;

    this->bottom = bottom;
}

void
rl_tile_cleanup(rl_tile *this)
{
    if (!this)
        return;

    free(this);
}

/******************************************************************************
rl_tilemap function implementations
******************************************************************************/

static void
rl_tilemap_check_glyph(rl_tilemap *this, wchar_t glyph)
{
    if (!this)
        return;

    if (!this->glyphs[(size_t)glyph] &&
        (this->glyphs[(size_t)glyph] = malloc(sizeof(sfGlyph))))
    {
        *(this->glyphs[(size_t)glyph]) = sfFont_getGlyph(this->font,
            (unsigned)glyph, (unsigned)this->chrsize, false, 0.0f);
    }
}

static void
rl_tilemap_gen_tile(rl_tilemap *this, rl_tile *tile, int x, int y)
{
    int index;
    sfIntRect rect;
    sfFloatRect bounds;
    float right = 0.0f, bottom = 0.0f;

    if (!this || !tile)
        return;

    rl_tilemap_check_glyph(this, tile->glyph);
    rect = this->glyphs[(size_t)(tile->glyph)]->textureRect;

    switch (tile->type)
    {
    case RL_TILE_TEXT:

        bounds = this->glyphs[(size_t)(tile->glyph)]->bounds;

        right = bounds.left;
        bottom = (float)(this->offsety) + bounds.top;
        break;

    case RL_TILE_EXACT:

        right = tile->right;
        bottom = tile->bottom;
        break;

    case RL_TILE_FLOOR:

        right = (float)((int)(((float)(this->offsetx) -
            (float)(rect.width))/2.0f));
        bottom = (float)((int)((float)(this->offsety) -
            (float)(rect.height)));
        break;

    case RL_TILE_CENTER:

        right = (float)((int)(((float)(this->offsetx) -
            (float)(rect.width))/2.0f));
        bottom = (float)((int)(((float)(this->offsety) -
            (float)(rect.height))/2.0f));
        break;

    }

    index = rl_tilemap_get_index(this, x, y);

    rl_tilemap_gen_fg(this, index, tile, x, y, right, bottom, &rect);
    rl_tilemap_gen_bg(this, index, tile, x, y);
}

static void
rl_tilemap_gen_fg(rl_tilemap *this, int index, rl_tile *tile, int x,
    int y, float r, float b, sfIntRect *rect)
{
    size_t vindex = (size_t)index * 4;

    if (!this || !tile || !rect)
        return;

    sfVertexArray_getVertex(this->fg, vindex)->position = (sfVector2f){
        (float)x * (float)(this->offsetx) + r,
        (float)y * (float)(this->offsety) + b
    };

    sfVertexArray_getVertex(this->fg, vindex + 1)->position = (sfVector2f){
        (float)x * (float)(this->offsetx) + (float)(rect->width) + r,
        (float)y * (float)(this->offsety) + b
    };

    sfVertexArray_getVertex(this->fg, vindex + 2)->position = (sfVector2f){
        (float)x * (float)(this->offsetx) + (float)(rect->width) + r,
        (float)y * (float)(this->offsety) + (float)(rect->height) + b
    };

    sfVertexArray_getVertex(this->fg, vindex + 3)->position = (sfVector2f){
        (float)x * (float)(this->offsetx) + r,
        (float)y * (float)(this->offsety) + (float)(rect->height) + b
    };

    sfVertexArray_getVertex(this->fg, vindex)->texCoords = (sfVector2f){
        (float)(rect->left),
        (float)(rect->top)
    };

    sfVertexArray_getVertex(this->fg, vindex + 1)->texCoords = (sfVector2f){
        (float)(rect->left) + (float)(rect->width),
        (float)(rect->top)
    };

    sfVertexArray_getVertex(this->fg, vindex + 2)->texCoords = (sfVector2f){
        (float)(rect->left) + (float)(rect->width),
        (float)(rect->top) + (float)(rect->height)
    };

    sfVertexArray_getVertex(this->fg, vindex + 3)->texCoords = (sfVector2f){
        (float)(rect->left),
        (float)(rect->top) + (float)(rect->height)
    };

    sfVertexArray_getVertex(this->fg, vindex)->color = tile->fg[0];
    sfVertexArray_getVertex(this->fg, vindex + 1)->color = tile->fg[1];
    sfVertexArray_getVertex(this->fg, vindex + 2)->color = tile->fg[2];
    sfVertexArray_getVertex(this->fg, vindex + 3)->color = tile->fg[3];
}

static void
rl_tilemap_gen_bg(rl_tilemap *this, int index, rl_tile *tile, int x,
    int y)
{
    size_t vindex = (size_t)index * 4;

    if (!this || !tile)
        return;

    sfVertexArray_getVertex(this->bg, vindex)->position = (sfVector2f){
        (float)x * (float)(this->offsetx),
        (float)y * (float)(this->offsety)
    };

    sfVertexArray_getVertex(this->bg, vindex + 1)->position = (sfVector2f){
        (float)x * (float)(this->offsetx) + (float)(this->offsetx),
        (float)y * (float)(this->offsety)
    };

    sfVertexArray_getVertex(this->bg, vindex + 2)->position = (sfVector2f){
        (float)x * (float)(this->offsetx) + (float)(this->offsetx),
        (float)y * (float)(this->offsety) + (float)(this->offsety)
    };

    sfVertexArray_getVertex(this->bg, vindex + 3)->position = (sfVector2f){
        (float)x * (float)(this->offsetx),
        (float)y * (float)(this->offsety) + (float)(this->offsety)
    };

    sfVertexArray_getVertex(this->bg, vindex)->color = tile->bg[0];
    sfVertexArray_getVertex(this->bg, vindex + 1)->color = tile->bg[1];
    sfVertexArray_getVertex(this->bg, vindex + 2)->color = tile->bg[2];
    sfVertexArray_getVertex(this->bg, vindex + 3)->color = tile->bg[3];
}

static int
rl_tilemap_get_index(rl_tilemap *this, int x, int y)
{
    if (!this)
        return 0;

    return (y * this->width) + x;
}

rl_tilemap *
rl_tilemap_create(const char *font, int chrsize, int width,
    int height, int offsetx, int offsety)
{
    rl_tilemap *this = NULL;

    if (!(this = malloc(sizeof(rl_tilemap))))
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

    sfVertexArray_resize(this->fg, (unsigned)(width * height * 4));
    sfVertexArray_setPrimitiveType(this->fg, sfQuads);

    if (!(this->bg = sfVertexArray_create()))
    {
        sfVertexArray_destroy(this->fg);
        sfFont_destroy(this->font);
        free(this);

        return NULL;
    }

    sfVertexArray_resize(this->bg, (unsigned)(width * height * 4));
    sfVertexArray_setPrimitiveType(this->bg, sfQuads);

    this->posx = 0.0f;
    this->posy = 0.0f;
    this->width = width;
    this->height = height;
    this->chrsize = chrsize;
    this->offsetx = offsetx;
    this->offsety = offsety;

    for (size_t i = 0; i < 65536; ++i)
        this->glyphs[i] = NULL;

    return this;
}

void
rl_tilemap_pos(rl_tilemap *this, float x, float y)
{
    if (!this)
        return;

    this->posx = x;
    this->posy = y;
}

void
rl_tilemap_tile(rl_tilemap *this, rl_tile *tile, int x, int y)
{
    if (!this || !tile)
        return;

    rl_tilemap_gen_tile(this, tile, x, y);
}

extern void
rl_tilemap_wstr_right(rl_tilemap *this, wchar_t *str, rl_color fg, rl_color bg,
    rl_tiletype type, int x, int y)
{
    rl_tile *tile = NULL;

    if (!this || !str)
        return;

    for (int i = 0; i < (int)wcslen(str); ++i)
    {
        if (!(tile = rl_tile_create(str[i], fg, bg, type, 0.0f, 0.0f)))
            return;

        rl_tilemap_gen_tile(this, tile, (x + i) % this->width,
            y + ((x + i) / this->width));

        rl_tile_cleanup(tile);
    }
}

extern void
rl_tilemap_wstr_down(rl_tilemap *this, wchar_t *str, rl_color fg, rl_color bg,
    rl_tiletype type, int x, int y)
{
    rl_tile *tile = NULL;

    if (!this || !str)
        return;

    for (int i = 0; i < (int)wcslen(str); ++i)
    {
        if (!(tile = rl_tile_create(str[i], fg, bg, type, 0.0f, 0.0f)))
            return;

        rl_tilemap_gen_tile(this, tile, x, y + i);

        rl_tile_cleanup(tile);
    }   
}

void
rl_tilemap_cleanup(rl_tilemap *this)
{
    if (!this)
        return;

    if (this->font)
        sfFont_destroy(this->font);

    if (this->fg)
        sfVertexArray_destroy(this->fg);

    if (this->bg)
        sfVertexArray_destroy(this->bg);

    for (size_t i = 0; i < 65536; ++i)
        if (this->glyphs[i]) free(this->glyphs[i]);

    if (this)
        free(this);
}

int
rl_tilemap_mouse_x(rl_tilemap *this, rl_display *disp)
{
    if (!this || !disp || !disp->window.handle)
        return 0;

    return rl_display_mouse_x(disp) / this->offsetx;
}

int
rl_tilemap_mouse_y(rl_tilemap *this, rl_display *disp)
{
    if (!this || !disp || !disp->window.handle)
        return 0;

    return rl_display_mouse_y(disp) / this->offsety;
}

void
rl_tilemap_mouse(rl_tilemap *this, rl_display *disp, int *x, int *y)
{
    if (!this || !disp || !disp->window.handle || !x || !y)
        return;

    rl_display_mouse(disp, x, y);
    
    *x /= this->offsetx;
    *y /= this->offsety;
}

