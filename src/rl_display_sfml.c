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

struct rltile {
    float right;
    float bottom;
    rlttype type;
    wchar_t glyph;
    sfColor fghue[4];
    sfColor bghue[4];
};

struct rltmap
{
    float x;
    float y;
    int offx;
    int offy;
    int cnum;
    int csize;
    int width;
    int height;
    sfFont *font;
    sfGlyph **glyphs;
    sfVertexArray *fg;
    sfVertexArray *bg;
};

struct rldisp
{
    struct {
        int width;
        int height;
        char *name;
        bool fscrn;
        sfRenderWindow *handle;
    } window;
    
    struct {
        int width;
        int height;
        sfColor clrhue;
        sfVector2f scale;
        sfRenderTexture *handle;
    } frame;
};

/******************************************************************************
Static global variables
******************************************************************************/

/* TODO: Tilemaps with the same csize should share their *glyphs arrays */

/* static sfGlyph glyphs[65536]; */

/******************************************************************************
Static function declarations
******************************************************************************/

/* misc */
static char *
strdup(const char *s);

/* rldisp */
static void
rldisp_updscl(rldisp *this);

static void
rldisp_rsizd(rldisp *this, int w, int h);

/* rltmap */
static void
rltmap_chkchr(rltmap *this, wchar_t c);

static int
rltmap_index(rltmap *this, int x, int y);

static void
rltmap_updtile(rltmap *this, rltile *t, int x, int y);

static void
rltmap_updbg(rltmap *this, int i, rltile *t, int x, int y);

static void
rltmap_updfg(rltmap *this, int i, rltile *t, int x, int y, float r,
    float b, sfIntRect *rect);

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
rldisp function implementations
******************************************************************************/

static void
rldisp_rsizd(rldisp *this, int width, int height)
{
    if (!this)
        return;

    this->window.width = width;
    this->window.height = height;

    rldisp_updscl(this);
}

static void
rldisp_updscl(rldisp *this)
{
    if (!this)
        return;

    this->frame.scale = (sfVector2f){
        (float)this->window.width / (float)this->frame.width,
        (float)this->window.height / (float)this->frame.height
    };
}

rldisp *
rldisp_init(int wwidth, int wheight, int fwidth, int fheight, char *name,
    bool fscrn)
{
    size_t mcount;
    rldisp *this = NULL;
    sfVideoMode mode = {(unsigned)wwidth, (unsigned)wheight, 32u};
    sfUint32 style = (fscrn) ? sfFullscreen : sfClose | sfTitlebar;

    /* If both wwidth and wheight are 0, then select the largest possible
       video mode to use */
    if (!wwidth && !wheight)
    {
        mode = sfVideoMode_getFullscreenModes(&mcount)[0];
    }

    /* TODO: Sanity check for size parameters? */

    if (!(this = malloc(sizeof(rldisp))))
        goto error;

    if (!(this->window.name = strdup(name)))
        goto error;

    if (!(this->window.handle = sfRenderWindow_create(mode, name, style,
        NULL)))
        goto error;

    this->window.fscrn = fscrn;
    this->window.width = wwidth;
    this->window.height = wheight;

    if (!(this->frame.handle = sfRenderTexture_create((unsigned)fwidth,
        (unsigned)fheight, false)))
        goto error;

    sfRenderWindow_setActive(this->window.handle, true);

    this->frame.width = fwidth;
    this->frame.height = fheight;
    this->frame.clrhue = sfBlack;

    rldisp_updscl(this);

    return this;

error:

    rldisp_free(this);
    return NULL;
}

void
rldisp_fscrn(rldisp *this, bool fscrn)
{
    sfUint32 style = (fscrn) ? sfFullscreen : sfClose | sfTitlebar;
    sfVideoMode mode = {(unsigned)this->window.width,
        (unsigned)this->window.height, 32};

    if (!this || !this->window.handle)
        return;

    this->window.fscrn = fscrn;

    sfRenderWindow_destroy(this->window.handle);
    this->window.handle = sfRenderWindow_create(mode, this->window.name, style,
        NULL);
}

void
rldisp_rsize(rldisp *this, int width, int height)
{
    sfVideoMode mode = {(unsigned)width, (unsigned)height, 32u};
    sfUint32 style = (this->window.fscrn) ? sfFullscreen
        : sfClose | sfTitlebar;

    if (!this || !this->window.handle)
        return;

    this->window.width = width;
    this->window.height = height;
    rldisp_updscl(this);

    sfRenderWindow_destroy(this->window.handle);
    this->window.handle = sfRenderWindow_create(mode, this->window.name, style,
        NULL);
}

void
rldisp_rname(rldisp *this, const char *name)
{
    if (!this || !this->window.name)
        return;

    free(this->window.name);

    this->window.name = strdup(name);

    sfRenderWindow_setTitle(this->window.handle, name);
}

void
rldisp_vsync(rldisp *this, bool enabled)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setVerticalSyncEnabled(this->window.handle, enabled);
}

void
rldisp_shwcur(rldisp *this, bool visible)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setMouseCursorVisible(this->window.handle, visible);
}

void
rldisp_fpslim(rldisp *this, int limit)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setFramerateLimit(this->window.handle, (unsigned)limit);
}

void
rldisp_free(rldisp *this)
{
    if (!this)
        return;

    if (this->frame.handle)
        sfRenderTexture_destroy(this->frame.handle);

    if (this->window.handle)
        sfRenderWindow_destroy(this->window.handle);

    if (this->window.name)
        free(this->window.name);

    if (this)
        free(this);
}

bool
rldisp_status(rldisp *this)
{
    if (!this || !this->window.handle)
        return false;

    return sfRenderWindow_isOpen(this->window.handle);
}

void
rldisp_evtflsh(rldisp *this)
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
                rldisp_rsizd(this, (int)evt.size.width,
                    (int)evt.size.height);
                break;
            default:
                break;
        }
    }
}

void
rldisp_clr(rldisp *this)
{
    if (!this || !this->window.handle)
        return;

    sfRenderTexture_clear(this->frame.handle, this->frame.clrhue);
}

void
rldisp_clrhue(rldisp *this, rlhue hue)
{
    if (!this)
        return;

    this->frame.clrhue = (sfColor){hue.r, hue.g, hue.b, hue.a};
}

void
rldisp_drtmap(rldisp *this, rltmap *tmap)
{
    sfRenderStates states;

    if (!this || !this->window.handle || !tmap)
        return;

    states.shader = NULL;
    states.blendMode = sfBlendAlpha;
    states.transform = sfTransform_Identity;
    sfTransform_translate(&states.transform, tmap->x, tmap->y);
    states.texture = sfFont_getTexture(tmap->font, (unsigned)tmap->csize);

    sfRenderTexture_drawVertexArray(this->frame.handle, tmap->bg, &states);
    sfRenderTexture_drawVertexArray(this->frame.handle, tmap->fg, &states);
}

void
rldisp_prsnt(rldisp *this)
{
    sfSprite *sprite = NULL;

    if (!this || !this->window.handle || !this->frame.handle
        || !(sprite = sfSprite_create()))
        return;

    sfRenderTexture_display(this->frame.handle);

    sfSprite_setScale(sprite, this->frame.scale);
    sfSprite_setTexture(sprite, sfRenderTexture_getTexture(this->frame.handle),
        false);

    sfRenderWindow_drawSprite(this->window.handle, sprite, NULL);
    sfRenderWindow_display(this->window.handle);
    sfSprite_destroy(sprite);
}

bool
rldisp_key(rldisp *this, rlkey key)
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
rldisp_mousx(rldisp *this)
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
rldisp_mousy(rldisp *this)
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
rldisp_mouse(rldisp *this, int *x, int *y)
{
    sfVector2i m;

    if (!this || !this->window.handle || !x || !y)
        return;

    m = sfMouse_getPositionRenderWindow(this->window.handle);

    if (m.x < 0 || m.x > this->window.width)
        *x = m.x;
    else
        *x = (int)((float)m.x / this->frame.scale.x);

    if (m.y < 0 || m.y > this->window.height)
        *y = m.y;
    else
        *y = (int)((float)m.y / this->frame.scale.y);
}

/******************************************************************************
rltile function implementations
******************************************************************************/

rltile *
rltile_init(wchar_t glyph, rlhue fghue, rlhue bghue, rlttype type, float right,
    float bottom)
{
    rltile *this = NULL;

    if (!(this = malloc(sizeof(rltile))))
        return NULL;

    this->glyph = glyph;
    
    for (int i = 0; i < 4; ++i)
    {
        this->fghue[i] = (sfColor){fghue.r, fghue.g, fghue.b, fghue.a};
        this->bghue[i] = (sfColor){bghue.r, bghue.g, bghue.b, bghue.a};
    }

    this->type = type;
    this->right = right;
    this->bottom = bottom;

    return this;
}

rltile *
rltile_null(void)
{
    rlhue fg = {255, 0, 0, 255};
    rlhue bg = {0, 0, 255, 255};

    return rltile_init(L'?', fg, bg, RL_TILE_CENTER, 0.0f, 0.0f);
}

void
rltile_glyph(rltile *this, wchar_t glyph)
{
    if (!this)
        return;

    this->glyph = glyph;
}

void
rltile_fghue(rltile *this, rlhue hue)
{
    if (!this)
        return;

    for (size_t i = 0; i < 4; ++i)
        this->fghue[i] = (sfColor){hue.r, hue.g, hue.b, hue.a};
}

void
rltile_bghue(rltile *this, rlhue hue)
{
    if (!this)
        return;

    for (size_t i = 0; i < 4; ++i)
        this->bghue[i] = (sfColor){hue.r, hue.g, hue.b, hue.a};
}

void
rltile_type(rltile *this, rlttype type)
{
    if (!this)
        return;

    this->type = type;
}

void
rltile_right(rltile *this, float right)
{
    if (!this)
        return;

    this->right = right;
}

void
rltile_bottm(rltile *this, float bottom)
{
    if (!this)
        return;

    this->bottom = bottom;
}

void
rltile_free(rltile *this)
{
    if (!this)
        return;

    free(this);
}

/******************************************************************************
rltmap function implementations
******************************************************************************/

static void
rltmap_chkchr(rltmap *this, wchar_t c)
{
    if (!this)
        return;

    if (!this->glyphs[(size_t)c] &&
        (this->glyphs[(size_t)c] = malloc(sizeof(sfGlyph))))
    {
        *(this->glyphs[(size_t)c]) = sfFont_getGlyph(this->font,
            (unsigned)c, (unsigned)this->csize, false, 0.0f);
    }
}

static void
rltmap_updtile(rltmap *this, rltile *t, int x, int y)
{
    int i;
    sfIntRect rect;
    sfFloatRect bnds;
    float r = 0.0f, b = 0.0f;

    if (!this || !t)
        return;

    rltmap_chkchr(this, t->glyph);
    rect = this->glyphs[(size_t)(t->glyph)]->textureRect;

    switch (t->type)
    {
    case RL_TILE_TEXT:
        bnds = this->glyphs[(size_t)(t->glyph)]->bounds;
        r = bnds.left;
        b = (float)(this->offy) + bnds.top;
        break;
    case RL_TILE_EXACT:
        r = t->right;
        b = t->bottom;
        break;
    case RL_TILE_FLOOR:
        r = (float)((int)(((float)(this->offx) -
            (float)(rect.width)) / 2.0f));
        b = (float)((int)((float)(this->offy) -
            (float)(rect.height)));
        break;
    case RL_TILE_CENTER:
        r = (float)((int)(((float)(this->offx) -
            (float)(rect.width)) / 2.0f));
        b = (float)((int)(((float)(this->offy) -
            (float)(rect.height)) / 2.0f));
        break;
    }

    i = rltmap_index(this, x, y);

    rltmap_updfg(this, i, t, x, y, r, b, &rect);
    rltmap_updbg(this, i, t, x, y);
}

static void
rltmap_updfg(rltmap *this, int i, rltile *t, int x, int y, float r, float b,
    sfIntRect *rect)
{
    size_t vi = (size_t)i * 4;

    if (!this || !t || !r)
        return;

    sfVertexArray_getVertex(this->fg, vi)->position = (sfVector2f){
        (float)x * (float)(this->offx) + r,
        (float)y * (float)(this->offy) + b
    };

    sfVertexArray_getVertex(this->fg, vi + 1)->position = (sfVector2f){
        (float)x * (float)(this->offx) + (float)(rect->width) + r,
        (float)y * (float)(this->offy) + b
    };

    sfVertexArray_getVertex(this->fg, vi + 2)->position = (sfVector2f){
        (float)x * (float)(this->offx) + (float)(rect->width) + r,
        (float)y * (float)(this->offy) + (float)(rect->height) + b
    };

    sfVertexArray_getVertex(this->fg, vi + 3)->position = (sfVector2f){
        (float)x * (float)(this->offx) + r,
        (float)y * (float)(this->offy) + (float)(rect->height) + b
    };

    sfVertexArray_getVertex(this->fg, vi)->texCoords = (sfVector2f){
        (float)(rect->left),
        (float)(rect->top)
    };

    sfVertexArray_getVertex(this->fg, vi + 1)->texCoords = (sfVector2f){
        (float)(rect->left) + (float)(rect->width),
        (float)(rect->top)
    };

    sfVertexArray_getVertex(this->fg, vi + 2)->texCoords = (sfVector2f){
        (float)(rect->left) + (float)(rect->width),
        (float)(rect->top) + (float)(rect->height)
    };

    sfVertexArray_getVertex(this->fg, vi + 3)->texCoords = (sfVector2f){
        (float)(rect->left),
        (float)(rect->top) + (float)(rect->height)
    };

    sfVertexArray_getVertex(this->fg, vi)->color = t->fghue[0];
    sfVertexArray_getVertex(this->fg, vi + 1)->color = t->fghue[1];
    sfVertexArray_getVertex(this->fg, vi + 2)->color = t->fghue[2];
    sfVertexArray_getVertex(this->fg, vi + 3)->color = t->fghue[3];
}

static void
rltmap_updbg(rltmap *this, int i, rltile *t, int x, int y)
{
    size_t vi = (size_t)i * 4;

    if (!this || !t)
        return;

    sfVertexArray_getVertex(this->bg, vi)->position = (sfVector2f){
        (float)x * (float)(this->offx),
        (float)y * (float)(this->offy)
    };

    sfVertexArray_getVertex(this->bg, vi + 1)->position = (sfVector2f){
        (float)x * (float)(this->offx) + (float)(this->offx),
        (float)y * (float)(this->offy)
    };

    sfVertexArray_getVertex(this->bg, vi + 2)->position = (sfVector2f){
        (float)x * (float)(this->offx) + (float)(this->offx),
        (float)y * (float)(this->offy) + (float)(this->offy)
    };

    sfVertexArray_getVertex(this->bg, vi + 3)->position = (sfVector2f){
        (float)x * (float)(this->offx),
        (float)y * (float)(this->offy) + (float)(this->offy)
    };

    sfVertexArray_getVertex(this->bg, vi)->color = t->bghue[0];
    sfVertexArray_getVertex(this->bg, vi + 1)->color = t->bghue[1];
    sfVertexArray_getVertex(this->bg, vi + 2)->color = t->bghue[2];
    sfVertexArray_getVertex(this->bg, vi + 3)->color = t->bghue[3];
}

static int
rltmap_index(rltmap *this, int x, int y)
{
    if (!this)
        return 0;

    return (y * this->width) + x;
}

rltmap *
rltmap_init(const char *font, int csize, int cnum, int width, int height,
    int offx, int offy)
{
    rltmap *this = NULL;

    if (!(this = malloc(sizeof(rltmap))))
        return NULL;

    if (!(this->font = sfFont_createFromFile(font)))
        goto error;

    if (!(this->fg = sfVertexArray_create()))
        goto error;

    sfVertexArray_resize(this->fg, (unsigned)(width * height * 4));
    sfVertexArray_setPrimitiveType(this->fg, sfQuads);

    if (!(this->bg = sfVertexArray_create()))
        goto error;

    sfVertexArray_resize(this->bg, (unsigned)(width * height * 4));
    sfVertexArray_setPrimitiveType(this->bg, sfQuads);

    if (!(this->glyphs = malloc(sizeof(sfGlyph *) * (long unsigned)cnum)))
        goto error;

    this->x = 0.0f;
    this->y = 0.0f;
    this->offx = offx;
    this->offy = offy;
    this->cnum = cnum;
    this->csize = csize;
    this->width = width;
    this->height = height;

    return this;

error:

    rltmap_free(this);
    return NULL;
}

void
rltmap_pos(rltmap *this, float x, float y)
{
    if (!this)
        return;

    this->x = x;
    this->y = y;
}

void
rltmap_tile(rltmap *this, rltile *tile, int x, int y)
{
    if (!this || !tile || tile->glyph > this->cnum)
        return;

    rltmap_updtile(this, tile, x, y);
}

extern void
rltmap_wstrr(rltmap *this, wchar_t *wstr, rlhue fg, rlhue bg, rlttype type,
    int x, int y)
{
    int xi, yi;
    rltile *tile = NULL;

    if (!this || !wstr)
        return;

    for (int i = 0; i < (int)wcslen(wstr); ++i)
    {
        xi = (x + i) % this->width;
        yi = y + ((x + i) / this->width);

        if (xi >= 0 && xi < this->width && yi >= 0 && yi < this->height)
        {
            if (!(tile = rltile_init(wstr[i], fg, bg, type, 0.0f, 0.0f)))
                return;
            rltmap_updtile(this, tile, xi, yi);
            rltile_free(tile);
        }
    }
}

extern void
rltmap_wstrb(rltmap *this, wchar_t *wstr, rlhue fg, rlhue bg, rlttype type,
    int x, int y)
{
    int xi, yi;
    rltile *tile = NULL;

    if (!this || !wstr)
        return;

    for (int i = 0; i < (int)wcslen(wstr); ++i)
    {
        xi = x + ((y + i) / this->height);
        yi = (y + i) % this->height;

        if (xi >= 0 && xi < this->width && yi >= 0 && yi < this->height)
        {
            if (!(tile = rltile_init(wstr[i], fg, bg, type, 0.0f, 0.0f)))
                return;
            rltmap_updtile(this, tile, xi, yi);
            rltile_free(tile);
        }
    }   
}

void
rltmap_free(rltmap *this)
{
    if (!this)
        return;

    if (this->font)
        sfFont_destroy(this->font);

    if (this->fg)
        sfVertexArray_destroy(this->fg);

    if (this->bg)
        sfVertexArray_destroy(this->bg);

    if (this->glyphs)
    {
        for (int i = 0; i < this->cnum; ++i)
            free(this->glyphs[i]);

        free(this->glyphs);
    }

    if (this)
        free(this);
}

int
rltmap_mousx(rltmap *this, rldisp *disp)
{
    if (!this || !disp || !disp->window.handle)
        return 0;

    return rldisp_mousx(disp) / this->offx;
}

int
rltmap_mousy(rltmap *this, rldisp *disp)
{
    if (!this || !disp || !disp->window.handle)
        return 0;

    return rldisp_mousy(disp) / this->offy;
}

void
rltmap_mouse(rltmap *this, rldisp *disp, int *x, int *y)
{
    if (!this || !disp || !disp->window.handle || !x || !y)
        return;

    rldisp_mouse(disp, x, y);
    
    *x /= this->offx;
    *y /= this->offy;
}

/******************************************************************************
rlhue function implementations
******************************************************************************/

extern void
rlhue_set(rlhue *this, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (!this)
        return;

    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

