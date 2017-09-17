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
    float r;
    float b;
    wchar_t chr;
    rlttype type;
    sfColor fg[4];
    sfColor bg[4];
};

struct rltmap
{
    int w;
    int h;
    float x;
    float y;
    int num;
    int offx;
    int offy;
    int chrsz;
    sfFont *font;
    sfGlyph **chrs;
    sfVertexArray *fg;
    sfVertexArray *bg;
};

struct rldisp
{
    struct {
        int w;
        int h;
        char *name;
        bool fscrn;
        sfRenderWindow *hndl;
    } win;
    
    struct {
        int w;
        int h;
        sfColor clrhue;
        sfVector2f scale;
        sfRenderTexture *hndl;
    } frm;
};

/******************************************************************************
Static global variables
******************************************************************************/

/* TODO: Tilemaps with the same chrsz should share their *chrs arrays */

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
rldisp_rsizd(rldisp *this, int w, int h)
{
    if (!this)
        return;

    this->win.w = w;
    this->win.h = h;

    rldisp_updscl(this);
}

static void
rldisp_updscl(rldisp *this)
{
    if (!this)
        return;

    this->frm.scale = (sfVector2f){
        (float)this->win.w / (float)this->frm.w,
        (float)this->win.h / (float)this->frm.h
    };
}

rldisp *
rldisp_init(int ww, int wh, int fw, int fh, char *n, bool f)
{
    size_t mc;
    rldisp *this = NULL;
    sfUint32 s = (f) ? sfFullscreen : sfClose | sfTitlebar;
    sfVideoMode m = {(unsigned)ww, (unsigned)wh, 32u};

    /* If both ww and wh are 0, then select the largest possible video
       mode to use */
    if (!ww && !wh)
    {
        m = sfVideoMode_getFullscreenModes(&mc)[0];
    }

    /* TODO: Sanity check for size parameters? */

    if (!(this = malloc(sizeof(rldisp))))
        goto error;

    if (!(this->win.name = strdup(n)))
        goto error;

    if (!(this->win.hndl = sfRenderWindow_create(m, n, s, NULL)))
        goto error;

    this->win.w = ww;
    this->win.h = wh;
    this->win.fscrn = f;

    if (!(this->frm.hndl = sfRenderTexture_create((unsigned)fw, (unsigned)fh,
        false)))
        goto error;

    sfRenderWindow_setActive(this->win.hndl, true);

    this->frm.w = fw;
    this->frm.h = fh;
    this->frm.clrhue = sfBlack;

    rldisp_updscl(this);

    return this;

error:

    rldisp_free(this);
    return NULL;
}

void
rldisp_fscrn(rldisp *this, bool f)
{
    sfUint32 s = (f) ? sfFullscreen : sfClose | sfTitlebar;
    sfVideoMode m = {(unsigned)this->win.w, (unsigned)this->win.h, 32};

    if (!this || !this->win.hndl)
        return;

    this->win.fscrn = f;

    sfRenderWindow_destroy(this->win.hndl);
    this->win.hndl = sfRenderWindow_create(m, this->win.name, s, NULL);
}

void
rldisp_rsize(rldisp *this, int w, int h)
{
    sfVideoMode m = {(unsigned)w, (unsigned)h, 32u};
    sfUint32 s = (this->win.fscrn) ? sfFullscreen : sfClose | sfTitlebar;

    if (!this || !this->win.hndl)
        return;

    this->win.w = w;
    this->win.h = h;
    rldisp_updscl(this);

    sfRenderWindow_destroy(this->win.hndl);
    this->win.hndl = sfRenderWindow_create(m, this->win.name, s, NULL);
}

void
rldisp_rname(rldisp *this, const char *n)
{
    if (!this || !this->win.name)
        return;

    free(this->win.name);

    this->win.name = strdup(n);

    sfRenderWindow_setTitle(this->win.hndl, n);
}

void
rldisp_vsync(rldisp *this, bool e)
{
    if (!this || !this->win.hndl)
        return;

    sfRenderWindow_setVerticalSyncEnabled(this->win.hndl, e);
}

void
rldisp_shwcur(rldisp *this, bool v)
{
    if (!this || !this->win.hndl)
        return;

    sfRenderWindow_setMouseCursorVisible(this->win.hndl, v);
}

void
rldisp_fpslim(rldisp *this, int l)
{
    if (!this || !this->win.hndl)
        return;

    sfRenderWindow_setFramerateLimit(this->win.hndl, (unsigned)l);
}

void
rldisp_free(rldisp *this)
{
    if (!this)
        return;

    if (this->frm.hndl)
        sfRenderTexture_destroy(this->frm.hndl);

    if (this->win.hndl)
        sfRenderWindow_destroy(this->win.hndl);

    if (this->win.name)
        free(this->win.name);

    if (this)
        free(this);
}

bool
rldisp_status(rldisp *this)
{
    if (!this || !this->win.hndl)
        return false;

    return sfRenderWindow_isOpen(this->win.hndl);
}

void
rldisp_evtflsh(rldisp *this)
{
    static sfEvent evt;

    if (!this || !this->win.hndl)
        return;

    while (sfRenderWindow_pollEvent(this->win.hndl, &evt))
    {
        switch (evt.type)
        {
            case sfEvtClosed:
                sfRenderWindow_close(this->win.hndl);
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
    if (!this || !this->win.hndl)
        return;

    sfRenderTexture_clear(this->frm.hndl, this->frm.clrhue);
}

void
rldisp_clrhue(rldisp *this, rlhue h)
{
    if (!this)
        return;

    this->frm.clrhue = (sfColor){h.r, h.g, h.b, h.a};
}

void
rldisp_drtmap(rldisp *this, rltmap *t)
{
    sfRenderStates s;

    if (!this || !this->win.hndl || !t)
        return;

    s.shader = NULL;
    s.blendMode = sfBlendAlpha;
    s.transform = sfTransform_Identity;
    sfTransform_translate(&s.transform, t->x, t->y);
    s.texture = sfFont_getTexture(t->font, (unsigned)t->chrsz);

    sfRenderTexture_drawVertexArray(this->frm.hndl, t->bg, &s);
    sfRenderTexture_drawVertexArray(this->frm.hndl, t->fg, &s);
}

void
rldisp_prsnt(rldisp *this)
{
    sfSprite *s = NULL;

    if (!this || !this->win.hndl || !this->frm.hndl || !(s= sfSprite_create()))
        return;

    sfRenderTexture_display(this->frm.hndl);

    sfSprite_setScale(s, this->frm.scale);
    sfSprite_setTexture(s, sfRenderTexture_getTexture(this->frm.hndl), false);

    sfRenderWindow_drawSprite(this->win.hndl, s, NULL);
    sfRenderWindow_display(this->win.hndl);
    sfSprite_destroy(s);
}

bool
rldisp_key(rldisp *this, rlkey k)
{
    if (!this || k < 0 || k >= RL_KEY_MAXIMUM)
        return false;

    /* *this is unused in the SFML renderer implementation, but the interface
       requires it to be passed for compatibility with other backends */
    UNUSED(this);

    switch (k)
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

    if (!this || !this->win.hndl)
        return 0;

    x = sfMouse_getPositionRenderWindow(this->win.hndl).x;

    if (x < 0 || x > this->win.w)
        return x;

    return (int)((float)x / this->frm.scale.x);
}

int
rldisp_mousy(rldisp *this)
{
    int y;

    if (!this || !this->win.hndl)
        return 0;

    y = sfMouse_getPositionRenderWindow(this->win.hndl).y;

    if (y < 0 || y > this->win.h)
        return y;

    return (int)((float)y / this->frm.scale.y);
}

void
rldisp_mouse(rldisp *this, int *x, int *y)
{
    sfVector2i m;

    if (!this || !this->win.hndl || !x || !y)
        return;

    m = sfMouse_getPositionRenderWindow(this->win.hndl);

    if (m.x < 0 || m.x > this->win.w)
        *x = m.x;
    else
        *x = (int)((float)m.x / this->frm.scale.x);

    if (m.y < 0 || m.y > this->win.h)
        *y = m.y;
    else
        *y = (int)((float)m.y / this->frm.scale.y);
}

/******************************************************************************
rltile function implementations
******************************************************************************/

rltile *
rltile_init(wchar_t c, rlhue fg, rlhue bg, rlttype t, float r, float b)
{
    rltile *this = NULL;

    if (!(this = malloc(sizeof(rltile))))
        return NULL;

    this->chr = c;
    
    for (int i = 0; i < 4; ++i)
    {
        this->fg[i] = (sfColor){fg.r, fg.g, fg.b, fg.a};
        this->bg[i] = (sfColor){bg.r, bg.g, bg.b, bg.a};
    }

    this->r = r;
    this->b = b;
    this->type = t;

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
rltile_chr(rltile *this, wchar_t c)
{
    if (!this)
        return;

    this->chr = c;
}

void
rltile_fghue(rltile *this, rlhue h)
{
    if (!this)
        return;

    for (size_t i = 0; i < 4; ++i)
        this->fg[i] = (sfColor){h.r, h.g, h.b, h.a};
}

void
rltile_bghue(rltile *this, rlhue h)
{
    if (!this)
        return;

    for (size_t i = 0; i < 4; ++i)
        this->bg[i] = (sfColor){h.r, h.g, h.b, h.a};
}

void
rltile_type(rltile *this, rlttype t)
{
    if (!this)
        return;

    this->type = t;
}

void
rltile_right(rltile *this, float r)
{
    if (!this)
        return;

    this->r = r;
}

void
rltile_bottm(rltile *this, float b)
{
    if (!this)
        return;

    this->b = b;
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

    if (!this->chrs[(size_t)c] &&
        (this->chrs[(size_t)c] = malloc(sizeof(sfGlyph))))
    {
        *(this->chrs[(size_t)c]) = sfFont_getGlyph(this->font,
            (unsigned)c, (unsigned)this->chrsz, false, 0.0f);
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

    rltmap_chkchr(this, t->chr);
    rect = this->chrs[(size_t)(t->chr)]->textureRect;

    switch (t->type)
    {
    case RL_TILE_TEXT:
        bnds = this->chrs[(size_t)(t->chr)]->bounds;
        r = bnds.left;
        b = (float)(this->offy) + bnds.top;
        break;
    case RL_TILE_EXACT:
        r = t->r;
        b = t->b;
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

    sfVertexArray_getVertex(this->fg, vi)->color = t->fg[0];
    sfVertexArray_getVertex(this->fg, vi + 1)->color = t->fg[1];
    sfVertexArray_getVertex(this->fg, vi + 2)->color = t->fg[2];
    sfVertexArray_getVertex(this->fg, vi + 3)->color = t->fg[3];
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

    sfVertexArray_getVertex(this->bg, vi)->color = t->bg[0];
    sfVertexArray_getVertex(this->bg, vi + 1)->color = t->bg[1];
    sfVertexArray_getVertex(this->bg, vi + 2)->color = t->bg[2];
    sfVertexArray_getVertex(this->bg, vi + 3)->color = t->bg[3];
}

static int
rltmap_index(rltmap *this, int x, int y)
{
    if (!this)
        return 0;

    return (y * this->w) + x;
}

rltmap *
rltmap_init(const char *f, int csz, int num, int w, int h, int offx, int offy)
{
    rltmap *this = NULL;

    if (!(this = malloc(sizeof(rltmap))))
        return NULL;

    if (!(this->font = sfFont_createFromFile(f)))
        goto error;

    if (!(this->fg = sfVertexArray_create()))
        goto error;

    sfVertexArray_resize(this->fg, (unsigned)(w * h * 4));
    sfVertexArray_setPrimitiveType(this->fg, sfQuads);

    if (!(this->bg = sfVertexArray_create()))
        goto error;

    sfVertexArray_resize(this->bg, (unsigned)(w * h * 4));
    sfVertexArray_setPrimitiveType(this->bg, sfQuads);

    if (!(this->chrs = malloc(sizeof(sfGlyph *) * (long unsigned)num)))
        goto error;

    this->w = w;
    this->h = h;
    this->x = 0.0f;
    this->y = 0.0f;
    this->num = num;
    this->chrsz = csz;
    this->offx = offx;
    this->offy = offy;

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
rltmap_tile(rltmap *this, rltile *t, int x, int y)
{
    if (!this || !t || t->chr > this->num)
        return;

    rltmap_updtile(this, t, x, y);
}

extern void
rltmap_wstrr(rltmap *this, wchar_t *s, rlhue fg, rlhue bg, rlttype t, int x,
    int y)
{
    int xi, yi;
    rltile *tile = NULL;

    if (!this || !s)
        return;

    for (int i = 0; i < (int)wcslen(s); ++i)
    {
        xi = (x + i) % this->w;
        yi = y + ((x + i) / this->w);

        if (xi >= 0 && xi < this->w && yi >= 0 && yi < this->h)
        {
            if (!(tile = rltile_init(s[i], fg, bg, t, 0.0f, 0.0f)))
                return;
            rltmap_updtile(this, tile, xi, yi);
            rltile_free(tile);
        }
    }
}

extern void
rltmap_wstrb(rltmap *this, wchar_t *s, rlhue fg, rlhue bg, rlttype t, int x,
    int y)
{
    int xi, yi;
    rltile *tile = NULL;

    if (!this || !s)
        return;

    for (int i = 0; i < (int)wcslen(s); ++i)
    {
        xi = x + ((y + i) / this->h);
        yi = (y + i) % this->h;

        if (xi >= 0 && xi < this->w && yi >= 0 && yi < this->h)
        {
            if (!(tile = rltile_init(s[i], fg, bg, t, 0.0f, 0.0f)))
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

    if (this->chrs)
    {
        for (int i = 0; i < this->num; ++i)
            free(this->chrs[i]);

        free(this->chrs);
    }

    if (this)
        free(this);
}

int
rltmap_mousx(rltmap *this, rldisp *d)
{
    if (!this || !d || !d->win.hndl)
        return 0;

    return rldisp_mousx(d) / this->offx;
}

int
rltmap_mousy(rltmap *this, rldisp *d)
{
    if (!this || !d || !d->win.hndl)
        return 0;

    return rldisp_mousy(d) / this->offy;
}

void
rltmap_mouse(rltmap *this, rldisp *d, int *x, int *y)
{
    if (!this || !d|| !d->win.hndl || !x || !y)
        return;

    rldisp_mouse(d, x, y);
    
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

