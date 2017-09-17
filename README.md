# Roguelike Display

Provides an interface for creating and managing a window as well as rendering
glyph data for text-centric graphical applications such as roguelike games. The
interface is generic across backends.

## Installation

For now, copy `src/rl_display.h` and the implementation file of your choosing
into your project. Currently only the CSFML implementation `src/rl_display_sfml.c`
is available, so you'll need to link to the CFML library. CSFML is available in
the package managers for most \*nix, homebrew on macOS, or can be downloaded
directly from the project's website prebuilt for Windows or the source code for
\*BSD: [link.](https://www.sfml-dev.org/download/csfml/)

Once stabilized, a header-only version of roguelike_display will be generated.

## Documentation

Doxygen documentation can be found inline in [src/rl_display.h](src/rl_display.h).

## Why

Because whenever I went to make a roguelike game, regardless of the window
management/rendering library I used, I'd eventually end up abstracting things
out into a similar API. I decided to standardize that API, so that in the
future I can cut down on boilerplate code. And by using C99, it should be easy
to generate bindings for other languages.

For the initial implementation I went with CSFML because I wanted to get
something up and running quickly. After I work with the API a bit and
stabilize it, I plan on implementing it with OpenGL/FreeType and then maybe
the SDL2 Renderer with SDL\_TTF.

## Example

```c
...

bool run = true;
rldisp *disp = NULL;
rltmap *tmap = NULL;
rltile *tile = NULL;
rlhue black = {0, 0, 0, 255};
rlhue red = {255, 0, 0, 255};
rlhue blue = {0, 0, 255, 255};
char *font = "res/fonts/unifont.ttf";

/* 1280x720 is the window size and 800x576 is the frame size which is stretched
 * to the window. The third parameter specifies the window title and the fourth
 * specifies whether or not the window is fullscreen.
 */
if (!(disp = rldisp_init(1280, 720, 800, 576, "roguelike display", false)))
    goto cleanup;

/* You can set many window options */
rldisp_fpslim(disp, 60);
rldisp_vsync(disp, true);
rldisp_shwcur(disp, true);
rldisp_clrhue(disp, black);

/* rltmaps are grids of characters with the same character size and spacing.
 * For this rltmap, the character size is 16, as well as the x and y spacing.
 * 50x36 are the dimensions of the map. 65536 is the maximum unicode character
 * value that should be supported.
 */
if (!(tmap = rltmap_init(font, 16, 65536, 50, 36, 16, 16)))
    goto cleanup;

/* When you create a tile, you specify the type. Either RL_TILE_TEXT,
 * RL_TILE_EXACT, RL_TILE_FLOOR or RL_TILE_CENTER. If the type is
 * RL_TILE_EXACT, then the last two parameters specify the right and down
 * offset of the glyph from the top-left.
 */
if (!(tile = rltile_init(L'╬', fg_color, bg_color, RLTILE_CENTER, 0.0f, 0.0f)))
    goto cleanup;

/* Update the rltmap's tile grid at coordinate 0x0 with data from tile */
rltmap_tile(tmap, tile, 0, 0);

while (rldisp_status(disp) && run)
{
    /* Handle all window events */
    rldisp_evtflsh(disp);

    /* Quit if escape is pressed */
    if (rldisp_key(disp, RL_KEY_ESCAPE))
        run = false;

    /* Clear the rldisp */
    rldisp_clr(disp);
    /* Draw the rltmap onto the rldisp's frame buffer */
    rldisp_drtmap(disp, tmap, 0.0f, 0.0f);
    /* Stretch the frame buffer to the window and finish rendering */
    rldisp_prsnt(disp);
}

...
```

#### A 50x36 grid of 16px random characters with random backgrounds and even spacing:
![example output](res/images/example0.png?raw=true)

#### An arbitrary number of layers can be added. Character size and spacing is adjustable:
![example output](res/images/example1.png?raw=true)

#### There are many options for spacing individual characters:
![example output](res/images/example2.png?raw=true)
