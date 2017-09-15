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

typedef struct rltile rltile;
typedef struct rltmap rltmap;
typedef struct rldisp rldisp;
typedef struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a; } rlhue;

/******************************************************************************
Enums
******************************************************************************/

typedef enum {
    RL_TILE_TEXT,
    RL_TILE_EXACT,
    RL_TILE_FLOOR,
    RL_TILE_CENTER
} rlttype;

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
} rlkey;

/******************************************************************************
rldisp function declarations
******************************************************************************/

extern rldisp *
rldisp_init(int ww, int wh, int fw, int fh, char *n, bool f);

extern void
rldisp_fscrn(rldisp *this, bool f);

extern void
rldisp_rsize(rldisp *this, int w, int h);

extern void
rldisp_rname(rldisp *this, const char *n);

extern void
rldisp_vsync(rldisp *this, bool e);

extern void
rldisp_shwcur(rldisp *this, bool v);

extern void
rldisp_fpslim(rldisp *this, int l);

extern void
rldisp_free(rldisp *this);

extern bool
rldisp_status(rldisp *this);

extern void
rldisp_evtflsh(rldisp *this);

extern void
rldisp_clr(rldisp *this);

extern void
rldisp_clrhue(rldisp *this, rlhue h);

extern void
rldisp_drtmap(rldisp *this, rltmap *t);

extern void
rldisp_prsnt(rldisp *this);

extern bool
rldisp_key(rldisp *this, rlkey k);

extern int
rldisp_mousx(rldisp *this);

extern int
rldisp_mousy(rldisp *this);

extern void
rldisp_mous(rldisp *this, int *x, int *y);

/******************************************************************************
rltile function declarations
******************************************************************************/

extern rltile *
rltile_init(wchar_t c, rlhue fg, rlhue bg, rlttype t, float r, float b);

extern rltile *
rltile_null(void);

extern void
rltile_chr(rltile *this, wchar_t c);

extern void
rltile_fg(rltile *this, rlhue h);

extern void
rltile_bg(rltile *this, rlhue h);

extern void
rltile_type(rltile *this, rlttype t);

extern void
rltile_right(rltile *this, float r);

extern void
rltile_bottm(rltile *this, float b);

extern void
rltile_free(rltile *this);

/******************************************************************************
rltmap function declarations
******************************************************************************/

extern rltmap *
rltmap_init(const char *f, int csz, int w, int h, int offx, int offy);

extern void
rltmap_pos(rltmap *this, float x, float y);

extern void
rltmap_tile(rltmap *this, rltile *t, int x, int y);

extern void
rltmap_wstrr(rltmap *this, wchar_t *s, rlhue fg, rlhue bg, rlttype t, int x,
    int y);

extern void
rltmap_wstrd(rltmap *this, wchar_t *s, rlhue fg, rlhue bg, rlttype t, int x,
    int y);

extern void
rltmap_free(rltmap *this);

extern int
rltmap_mousx(rltmap *this, rldisp *d);

extern int
rltmap_mousy(rltmap *this, rldisp *d);

extern void
rltmap_mous(rltmap *this, rldisp *d, int *x, int *y);

#ifdef __cplusplus
}
#endif

#endif /* RLDISPLAY_H */
