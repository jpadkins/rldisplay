#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "rl_display.h"

uint8_t rndcolor(void)
{
    return (uint8_t)(rand() % 255);
}

wchar_t rndascii(void)
{
    return (rand()%98) + 30;
}

wchar_t rnducode(void)
{
    return (rand()%65506) + 30;
}

int main(void)
{
    bool run = true;
    int mousex, mousey;
    int result = EXIT_SUCCESS;
    wchar_t *wstr = L"Hello World!\0";
    char *font = "res/fonts/unifont.ttf";

    rldisp *disp = NULL;
    rlhue color = { 0, 0, 0, 255 };
    rlhue fg = { 255, 255, 255, 255};
    rltmap *tmap = NULL, *curtmap = NULL;
    rltile *tile = NULL, *curtile = NULL;

    srand((unsigned)time(NULL));

    if (!(disp = rldisp_init(0, 0, 800, 576, "window", true)))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }

    rldisp_fpslim(disp, 60);
    rldisp_vsync(disp, true);
    rldisp_shwcur(disp, false);
    rldisp_clrhue(disp, color);

    if (!(tmap = rltmap_init(font, 16, 65536, 50, 36, 16, 16)))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }
    else if (!(tile = rltile_null()))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }

    for (int i = 0; i < 50; ++i)
    for (int j = 0; j < 36; ++j)
        rltmap_tile(tmap, tile, i, j);

    rltmap_wstrr(tmap, wstr, fg, color, RL_TILE_CENTER, 0, 0);
    rltmap_wstrb(tmap, wstr, fg, color, RL_TILE_CENTER, 0, 1);

    if (!(curtile = rltile_null()))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }

    if (!(curtmap = rltmap_init(font, 16, 65536, 1, 1, 16, 16)))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }

    rltile_glyph(curtile, L'⬉');
    rltile_type(curtile, RL_TILE_EXACT);
    rlhue_set(&color, 0, 0, 0, 255);
    rltile_bghue(curtile, color);
    rlhue_set(&color, 255, 255, 255, 255);
    rltile_fghue(curtile, color);
    rltile_right(curtile, 0.0f);
    rltile_bottm(curtile, 0.0f);
    rltmap_orign(curtmap, 8, 8);

    rltmap_tile(curtmap, curtile, 0, 0);

    while (rldisp_status(disp) && run)
    {
        rldisp_evtflsh(disp);

        if (rldisp_key(disp, RL_KEY_ESCAPE))
            run = false;

        if (rldisp_key(disp, RL_KEY_MOUSELEFT))
        {
            rltile_right(curtile, -3.0f);
            rltile_bottm(curtile, -3.0f);
            rltmap_tile(curtmap, curtile, 0, 0);
        }
        else
        {
            rltile_right(curtile, 0.0f);
            rltile_bottm(curtile, 0.0f);
            rltmap_tile(curtmap, curtile, 0, 0);
        }

        {
            static bool inc = true;
            static float rot = 0.0f;
            static float scale = 1.0f;
            static rlhue curhue = {170, 170, 170, 255};

            if (inc)
            {
                scale += 0.01f;
                rlhue_add(&curhue, 1, 1, 1, 0);
                rltile_fghue(curtile, curhue);
                if (curhue.r >= 255)
                    inc = false;
            }
            else
            {
                scale -= 0.01f;
                rlhue_sub(&curhue, 1, 1, 1, 0);
                rltile_fghue(curtile, curhue);
                if (curhue.r <= 170)
                    inc = true;
            }

            rot += 1.0f;
            rltmap_angle(curtmap, rot);
        }

        if (rldisp_key(disp, RL_KEY_SPACE))
        {
            for (int i = 0; i < 50; ++i)
            for (int j = 0; j < 36; ++j)
            {
                rlhue_set(&color, rndcolor(), rndcolor(), rndcolor(), 255);
                rltile_fghue(tile, color);
                rlhue_set(&color, rndcolor(), rndcolor(), rndcolor(), 255);
                rltile_bghue(tile, color);
                rltile_glyph(tile, rndascii());
                rltmap_tile(tmap, tile, i, j);
            }
        }

        rldisp_mouse(disp, &mousex, &mousey);
        rltmap_dpos(curtmap, mousex, mousey);

        rldisp_clr(disp);
        rldisp_drtmap(disp, tmap);
        rldisp_drtmap(disp, curtmap);
        rldisp_prsnt(disp);
    }

cleanup:

    if (disp) rldisp_free(disp);
    if (tile) rltile_free(tile);
    if (tmap) rltmap_free(tmap);
    if (curtile) rltile_free(curtile);
    if (curtmap) rltmap_free(curtmap);

    return result;
}

