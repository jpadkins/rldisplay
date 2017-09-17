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
    int result = EXIT_SUCCESS;
//  int mousex = -1, mousey = -1;
    wchar_t *wstr = L"Hello World!\0";
    char *font = "res/fonts/unifont.ttf";

    rltile *tile = NULL;
    rldisp *disp = NULL;
    rltmap *tmap = NULL;
    rlhue color = { 0, 0, 0, 255 };
    rlhue fg = { 255, 255, 255, 255};

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

                rltile_wchr(tile, rnducode());

                rltmap_tile(tmap, tile, i, j);
            }
            }
        }
/*
        rltmap_mouse(tmap, disp, &mousex, &mousey);

        if (mousex >= 0 && mousex < 50 && mousey >= 0 && mousey < 36)
        {
            rlhue_set(&color, rndcolor(), rndcolor(), rndcolor(), 255);

            rltile_fghue(tile, color);

            rlhue_set(&color, rndcolor(), rndcolor(), rndcolor(), 255);

            rltile_bghue(tile, color);

            rltile_chr(tile, (wchar_t)(rand() % 65536));

            rltmap_tile(tmap, tile, mousex, mousey);
        }
*/
        rldisp_clr(disp);
        rldisp_drtmap(disp, tmap);
        rldisp_prsnt(disp);
    }

cleanup:
    if (tile) rltile_free(tile);
    if (tmap) rltmap_free(tmap);
    if (disp) rldisp_free(disp);

    return result;
}

