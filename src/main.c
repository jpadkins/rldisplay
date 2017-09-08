#include <time.h>
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
    uint8_t color[4] = { 0, 255, 0, 255 };

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

    for (uint32_t j = 0; j < 36; ++j)
    {
        for (uint32_t i = 0; i < 50; ++i)
        {
            RLTile_set_glyph(tile, (wchar_t)(rand() % 65536));

            for (uint32_t k = 0; k < 3; ++k)
                color[k] = (uint8_t)(rand() % 255);

            RLTile_set_fg(tile, color);

            for (uint32_t k = 0; k < 3; ++k)
                color[k] = (uint8_t)(rand() % 255);

            RLTile_set_bg(tile, color);

            RLTileMap_put_tile(tmap, tile, i, j);
        }
    }

    while (RLDisplay_status(disp) && run)
    {
        RLDisplay_events_flush(disp);

        if (RLDisplay_key_pressed(disp, RLDISPLAY_KEY_ESCAPE))
            run = false;

        if (RLDisplay_key_pressed(disp, RLDISPLAY_KEY_SPACE))
        {
            for (uint32_t j = 0; j < 36; ++j)
            {
                for (uint32_t i = 0; i < 50; ++i)
                {
                    RLTile_set_glyph(tile, (wchar_t)(rand() % 65536));

                    for (uint32_t k = 0; k < 3; ++k)
                        color[k] = (uint8_t)(rand() % 255);

                    RLTile_set_fg(tile, color);

                    for (uint32_t k = 0; k < 3; ++k)
                        color[k] = (uint8_t)(rand() % 255);

                    RLTile_set_bg(tile, color);

                    RLTileMap_put_tile(tmap, tile, i, j);
                }
            }
        }

        RLDisplay_clear(disp);
        RLDisplay_draw_tilemap(disp, tmap, 0.0f, 0.0f);
        RLDisplay_present(disp);
    }

cleanup:
    if (tile) RLTile_cleanup(tile);
    if (tmap) RLTileMap_cleanup(tmap);
    if (disp) RLDisplay_cleanup(disp);

    return result;
}

