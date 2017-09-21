#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "rl_display.h"

#define UNUSED(x) (void)x

void
view_set(rltmap *view, rltile *tile)
{
    rlhue hue;
    int housex, housey;

    if (!view || !tile)
        return;
    
    for (int i = 0; i < 30; ++i)
    for (int j = 0; j < 30; ++j)
    {
        if (rand()%15 == 0)
        {
            rltile_glyph(tile, L'♠');
            rlhue_set(&hue, 0, (uint8_t)(rand()%50 + 140), 0, 255);
            rltile_fghue(tile, hue);
        }
        else
        {
            if (rand()%3 == 0)
                rltile_glyph(tile, L',');
            else
                rltile_glyph(tile, L'.');

            rlhue_set(&hue, 0, (uint8_t)(rand()%20 + 70), 0, 255);
            rltile_fghue(tile, hue);
        }
           
        rlhue_set(&hue, 0, (uint8_t)(rand()%20 + 20), 0, 255);
        rltile_bghue(tile, hue);
        rltmap_ptile(view, tile, i, j);
    }

    rltile_glyph(tile, L'☰');
    rlhue_set(&hue, 100, 20, 20, 255);
    rltile_fghue(tile, hue);
    rlhue_set(&hue, 50, 10, 10, 255);
    rltile_bghue(tile, hue);

    housex = rand()%10 + 4;
    housey = rand()%10 + 4;

    for (int i = housex; i < housex + 14; ++i)
    {
        rltmap_ptile(view, tile, i, housey);
        rltmap_ptile(view, tile, i, housey + 13);
    }
    for (int i = housey; i < housey + 14; ++i)
    {
        rltmap_ptile(view, tile, housex, i);
        rltmap_ptile(view, tile, housex + 13, i);
    }

    rltile_glyph(tile, L' ');

    for (int i = housex + 1; i < housex + 13; ++i)
    for (int j = housey + 1; j < housey + 13; ++j)
        rltmap_ptile(view, tile, i, j);
}

void
menu_set(rltmap *menu, rltile *tile)
{
    rlhue black = {0, 0, 0, 255};
    rlhue white = {255, 255, 255, 255};

    if (!menu || !tile)
        return;

    rltile_glyph(tile, L'#');
    rltile_fghue(tile, white);
    rltile_bghue(tile, black);

    for (int i = 0; i < 20; ++i)
    {
        rltmap_ptile(menu, tile, i, 0);
        rltmap_ptile(menu, tile, i, 29);
    }

    for (int i = 0; i < 30; ++i)
    {
        rltmap_ptile(menu, tile, 0, i);
        rltmap_ptile(menu, tile, 19, i);
    }

    rltile_glyph(tile, L' ');

    for (int i = 1; i < 19; ++i)
    for (int j = 1; j < 29; ++j)
        rltmap_ptile(menu, tile, i, j);
}

void
curs_set(rltmap *curs, rltile *tile)
{
    rlhue hue;

    if (!curs || !tile)
        return;

    rltile_glyph(tile, L'⬉');
    rlhue_set(&hue, 255, 255, 255, 255);
    rltile_fghue(tile, hue);
    rlhue_set(&hue, 0, 0, 0, 0);
    rltile_bghue(tile, hue);
    rltmap_ptile(curs, tile, 0, 0);
}

int
main(void)
{
    bool run = true;
    double delta = 0.0;
    rldisp *disp = NULL;
    rltile *tile = NULL;
    int mousex = 0, mousey = 0;
    const char *font = "res/fonts/unifont.ttf";
    rltmap *view = NULL, *menu = NULL, *curs = NULL;

    if (!(disp = rldisp_init(0, 0, 640, 480, "rldisplay", true)))
        goto cleanup;

    if (!(view = rltmap_init(font, 16, 65536, 30, 30, 16, 16)))
        goto cleanup;

    if (!(menu = rltmap_init(font, 16, 65536, 20, 30, 8, 16)))
        goto cleanup;

    if (!(curs = rltmap_init(font, 32, 65536, 1, 1, 32, 32)))
        goto cleanup;

    if (!(tile = rltile_null()))
        goto cleanup;

    rltmap_dpos(view, 160, 0);

    rldisp_fpslim(disp, 60);
    rldisp_vsync(disp, true);
    rldisp_shwcur(disp, false);

    view_set(view, tile);
    menu_set(menu, tile);
    curs_set(curs, tile);

    while (rldisp_status(disp) && run)
    {
        delta += rldisp_delta();

        rldisp_evtflsh(disp);
        rldisp_mouse(disp, &mousex, &mousey);

        if (rldisp_key(disp, RL_KEY_ESCAPE))
            run = false;

        if (rldisp_key(disp, RL_KEY_SPACE))
            view_set(view, tile);

        rltmap_dpos(curs, mousex, mousey);

        rldisp_clear(disp);
        rldisp_dtmap(disp, menu);
        rldisp_dtmap(disp, view);
        rldisp_dtmap(disp, curs);
        rldisp_prsnt(disp);
    }

cleanup:

    rldisp_free(disp);
    rltile_free(tile);
    rltmap_free(view);
    rltmap_free(menu);
    rltmap_free(curs);
}
