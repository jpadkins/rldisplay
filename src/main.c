#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rl_display.h"

int main(void)
{
    bool run = true;
    RLTile *tile = NULL;
    RLDisplay *disp = NULL;
    RLTileMap *tmap = NULL;
    int result = EXIT_SUCCESS;
    uint8_t color[4] = { 0, 0, 0, 255 };

    srand((unsigned)time(NULL));

    if (!(disp = RLDisplay_create(3840, 2160, "window", true, 800, 576)))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }

    RLDisplay_vsync(disp, true);
    RLDisplay_cursor(disp, false);
    RLDisplay_framerate(disp, 60);
    RLDisplay_clear_color(disp, color);

    if (!(tmap = RLTileMap_create("res/fonts/unifont.ttf", 16, 16, 16, 50,
        36)))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }
    else if (!(tile = RLTile_default()))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }

    while (RLDisplay_status(disp) && run)
    {
        RLDisplay_events_flush(disp);

        if (RLDisplay_key_pressed(disp, RLDISPLAY_KEY_ESCAPE))
            run = false;

        RLDisplay_clear(disp);
        RLDisplay_draw_tilemap(disp, tmap, 0.0f, 0.0f);
        RLDisplay_present(disp);
    }

cleanup:
    if (tmap) RLTileMap_cleanup(tmap);
    if (disp) RLDisplay_cleanup(disp);
    if (tile) RLTile_cleanup(tile);

    return result;
}

