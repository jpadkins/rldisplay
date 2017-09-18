#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
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

    rltile *tile = NULL;
    rldisp *disp = NULL;
    rlhue color = { 0, 0, 0, 255 };
    rlhue fg = { 255, 255, 255, 255};
    rltmap *tmap = NULL, *cursor = NULL;

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
    {
        for (int j = 0; j < 36; ++j)
        {
            rltmap_tile(tmap, tile, i, j);
        }
    }

    rltmap_wstrr(tmap, wstr, fg, color, RL_TILE_CENTER, 0, 0);
    rltmap_wstrb(tmap, wstr, fg, color, RL_TILE_CENTER, 0, 1);

    rltile_glyph(tile, L'↖');
    rltile_type(tile, RL_TILE_EXACT);
    rlhue_set(&color, 0, 0, 0, 0);
    rltile_bghue(tile, color);
    rlhue_set(&color, 255, 255, 255, 255);
    rltile_fghue(tile, color);
    rltile_right(tile, -9.0f);
    rltile_bottm(tile, -9.0f);

    if (!(cursor = rltmap_init(font, 32, 65536, 1, 1, 32, 32)))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }

    rltmap_tile(cursor, tile, 0, 0);
    rltile_type(tile, RL_TILE_CENTER);

    while (rldisp_status(disp) && run)
    {
        rldisp_evtflsh(disp);

        if (rldisp_key(disp, RL_KEY_ESCAPE))
            run = false;

        if (rldisp_key(disp, RL_KEY_SPACE))
        {
            for (int i = 0; i < 50; ++i)
            {
            for (int j = 0; j < 36; ++j)
            {
                rlhue_set(&color, rndcolor(), rndcolor(), rndcolor(), 255);

                rltile_fghue(tile, color);

                rlhue_set(&color, rndcolor(), rndcolor(), rndcolor(), 255);

                rltile_bghue(tile, color);

                rltile_glyph(tile, rnducode());

                rltmap_tile(tmap, tile, i, j);
            }
            }
        }

        rldisp_mouse(disp, &mousex, &mousey);
        rltmap_dpos(cursor, mousex, mousey);

        rldisp_clr(disp);
        rldisp_drtmap(disp, tmap);
        rldisp_drtmap(disp, cursor);
        rldisp_prsnt(disp);
    }

cleanup:
    if (tile) rltile_free(tile);
    if (cursor) rltmap_free(cursor);
    if (tmap) rltmap_free(tmap);
    if (disp) rldisp_free(disp);

    return result;
}

