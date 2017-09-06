#include <stdlib.h>
#include <stdbool.h>
#include "display.h"

int main(void)
{
    bool run = true;
    struct display *disp = NULL;

    if (!(disp = display_create(800, 600, "window", false)))
        return EXIT_FAILURE;

    display_vsync(disp, true);
    display_cursor(disp, false);
    display_framerate(disp, 60);

    while (display_status(disp) && run)
    {
        display_events_flush(disp);

        if (display_key_pressed(disp, DISPLAY_KEY_ESCAPE))
            run = false;

        display_clear(disp);
        display_present(disp);
    }

    display_cleanup(disp);

    return EXIT_SUCCESS;
}

