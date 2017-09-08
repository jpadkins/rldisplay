# Roguelike Display

Provides an interface for creating and managing a window as well as rendering
glyph data for text-centric graphical applications such as roguelike games. The
interface is generic across backends.

## Installation

For now, copy `src/rl_display.h` and the implementation file of your choosing
into your project. Currently only the CSFML implementation
(`src/rl_display_sfml.c`) is available.

Once stabilized, a header-only version of the library will be generated.

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
RLTile *tile = NULL;
RLDisplay *disp = NULL;
RLTileMap *tmap = NULL;
uint8_t fg_color[4] = {255, 0, 0, 255};
uint8_t bg_color[4] = {0, 0, 255, 255};

if (!(disp = RLDisplay_create(1280, 720, "title", false, 800, 576)))
    goto cleanup;

else if (!(tmap = RLTileMap_create("res/fonts/unifont.ttf", 16, 16, 16, 50, 36)))
    goto cleanup;

else if (!(tile = RLTile_create(L'╬', fg_color, bg_color, RLTILE_CENTER, 0.0f, 0.0f)))
    goto cleanup;

RLTileMap_put_tile(tmap, tile, 0, 0);

while (RLDisplay_status(disp) && tun)
{
    RLDisplay_events_flush(disp);

    if (RLDisplay_key_pressed(disp, RLDISPLAY_KEY_ESCAPE))
        run = false;

    RLDisplay_clear(disp);
    RLDisplay_draw_tilemap(disp, tmap, 0.0f, 0.0f);
    RLDisplay_present(disp);
}

...
```

#### A 50x36 grid of 16px random characters with random backgrounds and even spacing:
![example output](res/images/example0.png?raw=true)

#### An arbitrary number of layers can be added. Character size and spacing is adjustable.
![example output](res/images/example1.png?raw=true)

#### There are many options for spacing individual characters.
![example output](res/images/example2.png?raw=true)
