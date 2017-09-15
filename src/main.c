#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rldisp.h"

int main(void)
{
    bool run = true;
    int result = EXIT_SUCCESS;
    int mousex = -1, mousey = -1;
    wchar_t *wstr = L"Hello World!\0";

    rltile *tile = NULL;
    rldisp *disp = NULL;
    rltmap *tmap = NULL;
    rlhue color = { 0, 0, 0, 255 };
    rlhue fg = { 255, 255, 255, 255};

    srand((unsigned)time(NULL));

    if (!(disp = rldisp_init(1280, 720, 800, 576, "window", false)))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }

    rldisp_fpslim(disp, 60);
    rldisp_vsync(disp, true);
    rldisp_shwcur(disp, true);
    rldisp_clrhue(disp, color);

    if (!(tmap = rltmap_init("res/fonts/unifont.ttf", 16, 50, 36, 16, 16)))
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

    rltmap_wstrd(tmap, wstr, fg, color, RL_TILE_CENTER, 0, 1);
    rltmap_wstrr(tmap, wstr, fg, color, RL_TILE_CENTER, 0, 0);

    while (rldisp_status(disp) && run)
    {
        rldisp_evtflsh(disp);

        if (rldisp_key(disp, RL_KEY_ESCAPE))
            run = false;

        rltmap_mous(tmap, disp, &mousex, &mousey);

        if (mousex >= 0 && mousex < 50 && mousey >= 0 && mousey < 36)
        {
            color.r = (uint8_t)(rand() % 255);
            color.g = (uint8_t)(rand() % 255);
            color.b = (uint8_t)(rand() % 255);

            rltile_fg(tile, color);

            color.r = (uint8_t)(rand() % 255);
            color.g = (uint8_t)(rand() % 255);
            color.b = (uint8_t)(rand() % 255);

            rltile_bg(tile, color);

            rltile_chr(tile, (wchar_t)(rand() % 128));

            rltmap_tile(tmap, tile, mousex, mousey);
        }

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

