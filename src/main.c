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
    int result = EXIT_SUCCESS;
    int mousex = -1, mousey = -1;

    rl_tile *tile = NULL;
    rl_display *disp = NULL;
    rl_tilemap *tmap = NULL;
    rl_color color = { 0, 0, 0, 255 };

    srand((unsigned)time(NULL));

    if (!(disp = rl_display_create(1280, 720, 800, 576, "window", false)))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }

    rl_display_vsync(disp, true);
    rl_display_cursor(disp, true);
    rl_display_fps_limit(disp, 60);
    rl_display_clear_color(disp, color);

    if (!(tmap = rl_tilemap_create("res/fonts/unifont.ttf", 16, 50, 36, 16,
        16)))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }
    else if (!(tile = rl_tile_default()))
    {
        result = EXIT_FAILURE;
        goto cleanup;
    }

    for (int i = 0; i < 50; ++i)
    {
        for (int j = 0; j < 36; ++j)
        {
            rl_tilemap_put_tile(tmap, tile, i, j);
        }
    }

    while (rl_display_status(disp) && run)
    {
        rl_display_events_flush(disp);

        if (rl_display_key_pressed(disp, RL_DISPLAY_KEY_ESCAPE))
            run = false;

        rl_tilemap_mouse(tmap, disp, &mousex, &mousey);

        if (mousex >= 0 && mousex < 50 && mousey >= 0 && mousey < 36)
        {
            color.r = (uint8_t)(rand() % 255);
            color.g = (uint8_t)(rand() % 255);
            color.b = (uint8_t)(rand() % 255);

            rl_tile_fg(tile, color);

            color.r = (uint8_t)(rand() % 255);
            color.g = (uint8_t)(rand() % 255);
            color.b = (uint8_t)(rand() % 255);

            rl_tile_bg(tile, color);

            rl_tile_glyph(tile, (wchar_t)(rand() % 65536));

            rl_tilemap_put_tile(tmap, tile, rl_tilemap_mouse_x(tmap, disp),
                rl_tilemap_mouse_y(tmap, disp));
        }

        rl_display_clear(disp);
        rl_display_draw_tilemap(disp, tmap);
        rl_display_present(disp);
    }

cleanup:
    if (tmap) rl_tilemap_cleanup(tmap);
    if (disp) rl_display_cleanup(disp);
    if (tile) rl_tile_cleanup(tile);

    return result;
}

