#ifndef RL_DISPLAY_H
#define RL_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
Structs
******************************************************************************/

typedef struct rltile rltile;
typedef struct rltmap rltmap;
typedef struct rldisp rldisp;
typedef struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a; } rlhue;

/******************************************************************************
Enums
******************************************************************************/

typedef enum {
    RL_TILE_TEXT,
    RL_TILE_EXACT,
    RL_TILE_FLOOR,
    RL_TILE_CENTER
} rlttype;

typedef enum {
    RL_KEY_A,
    RL_KEY_B,
    RL_KEY_C,
    RL_KEY_D,
    RL_KEY_E,
    RL_KEY_F,
    RL_KEY_G,
    RL_KEY_H,
    RL_KEY_I,
    RL_KEY_J,
    RL_KEY_K,
    RL_KEY_L,
    RL_KEY_M,
    RL_KEY_N,
    RL_KEY_O,
    RL_KEY_P,
    RL_KEY_Q,
    RL_KEY_R,
    RL_KEY_S,
    RL_KEY_T,
    RL_KEY_U,
    RL_KEY_V,
    RL_KEY_W,
    RL_KEY_X,
    RL_KEY_Y,
    RL_KEY_Z,
    RL_KEY_0,
    RL_KEY_1,
    RL_KEY_2,
    RL_KEY_3,
    RL_KEY_4,
    RL_KEY_5,
    RL_KEY_6,
    RL_KEY_7,
    RL_KEY_8,
    RL_KEY_9,
    RL_KEY_ESCAPE,
    RL_KEY_CONTROL,
    RL_KEY_SHIFT,
    RL_KEY_ALT,
    RL_KEY_SYSTEM,
    RL_KEY_SEMICOLON,
    RL_KEY_COMMA,
    RL_KEY_PERIOD,
    RL_KEY_QUOTE,
    RL_KEY_SLASH,
    RL_KEY_TILDE,
    RL_KEY_SPACE,
    RL_KEY_ENTER,
    RL_KEY_BACKSPACE,
    RL_KEY_TAB,
    RL_KEY_UP,
    RL_KEY_DOWN,
    RL_KEY_LEFT,
    RL_KEY_RIGHT,
    RL_KEY_MOUSELEFT,
    RL_KEY_MOUSERIGHT,
    RL_KEY_MOUSEMIDDLE,

    RL_KEY_MAXIMUM
} rlkey;

/******************************************************************************
rldisp function declarations
******************************************************************************/

/* @brief   Returns a pointer to a new rldisp (window)
 *
 * The window dimensions determine the size of the actual window. They may
 * only be changed later with rldisp_rsize(3). The frame buffer dimensions
 * are what should be taken into account when using mouse coords and drawing
 * to the window, as the frame is what is actually drawn to. Every time the
 * window is refreshed with rldisp_prsnt(1), the frame is stretched to the
 * current size of the window. If the window is fullscreen, but the passed
 * window dimensions are not a valid fullscreen mode, the largest valid mode
 * will be selected. If you just want to create a fullscreen window at the
 * largest possible mode for the monitor, you can pass 0 as the first two
 * arguments and true as the last.
 *
 * @param   wwidth  width of the window
 * @param   wheight height of the window
 * @param   fwidth  width of the frame buffer
 * @param   fheight height of the frame buffer
 * @param   name    name of the window
 * @param   fscrn   whether the window is fullscreen
 *
 * @return  a pointer to the new rldisp
 */
extern rldisp *
rldisp_init(int wwidth, int wheight, int fwidth, int fheight, char *name,
    bool fscrn);

/* @brief   Sets an rldisp to be either fullscreen or windowed.
 *
 * Calling this function will cause the window to be recreated.
 *
 * @param   this    pointer to an rldisp
 * @param   fscrn   whether the window should be set to fullscreen or windowed
 */
extern void
rldisp_fscrn(rldisp *this, bool fscrn);

/* @brief   Sets an rldisp window size
 *
 * Calling this function will cause the window to be recreated. If the window
 * is fullscreen and the dimensions passed are not a valid fullscreen mode,
 * the largest valid mode will be selected.
 *
 * @param   this    pointer to an rldisp
 * @param   width   new width of the window
 * @param   height  new height of the window
 */
extern void
rldisp_rsize(rldisp *this, int width, int height);

/* @brief   Sets an rldisp window name
 *
 * @param   this    pointer to an rldisp
 * @param   name    new window name
 */
extern void
rldisp_rname(rldisp *this, const char *name);

/* @brief   Sets whether or not an rldisp window should enable vertical sync
 *
 * @param   this    pointer to an rldisp
 * @param   enabled whether vertical sync is enabled
 */
extern void
rldisp_vsync(rldisp *this, bool enabled);

/* @brief   Sets whther or not an rldisp window should hide the mouse cursor
 *
 * @param   this    pointer to an rldisp
 * @param   visible whether the mouse cursor is visible
 */
extern void
rldisp_shwcur(rldisp *this, bool visible);

/* @brief   Sets the fps limit of an rldisp window (or 0 to disable)
 *
 * The default value for an rldisp is 0 (disabled).
 *
 * @param   this    pointer to an rldisp
 * @param   limit   new fps limit
 */
extern void
rldisp_fpslim(rldisp *this, int limit);

/* @brief   Frees the memory allocated for an rldisp
 *
 * @param   this    pointer to an rldisp
 */
extern void
rldisp_free(rldisp *this);

/* @brief   Returns the status of an rldisp window
 *
 * @param   this    pointer to an rldisp
 *
 * @return  true if the window is open, false otherwise
 */
extern bool
rldisp_status(rldisp *this);

/* @brief   Flushes the input event queue for an rldisp window
 *
 * This should be called every frame, so that rldisp_key(2) returns the
 * correct value.
 *
 * @param   this    pointer to an rldisp
 */
extern void
rldisp_evtflsh(rldisp *this);

/* @brief   Clears an rldisp window
 *
 * Clears the rldisp window to the hue set by rldisp_clrhue(2). You should
 * begin every discrete frame by calling this function. The default hue is
 * black.
 *
 * @param   this    pointer to an rldisp
 */
extern void
rldisp_clr(rldisp *this);

/* @brief   Sets the clear hue for an rldisp window
 *
 * @param   this    pointer to the rldisp
 * @param   hue     rlhue that the clear hue is set to
 */
extern void
rldisp_clrhue(rldisp *this, rlhue hue);

/* @brief   Draws a rltmap into the rldisp's frame buffer
 *
 * @param   this    pointer to an rldisp
 * @param   tmap    pointer to an rltmap
 */
extern void
rldisp_drtmap(rldisp *this, rltmap *tmap);

/* @brief   Draws a line directly to an rldisp's frame buffer
 *
 * The coordinates should be based on the rldisp's frame buffer, not its window
 * size.
 *
 * @param   this    pointer to the rldisp
 * @param   x0      x coordinate of the start of the line
 * @param   y0      y coordinate of the start of the line
 * @param   x1      x coordinate of the end of the line
 * @param   y1      y coordinate of the end of the line
 * @param   thick   thickness of the line
 * @param   hue     hue of the line
 */
extern void
rldisp_drline(rldisp *this, int x0, int y0, int x1, int y1, int thick,
    rlhue hue);

/* @brief   Draws an outward empty box directly to an rldisp's frame buffer
 * 
 * The coordinates should be based on the rldisp's frame buffer, not its window
 * size. The thickness of the box moves outward from the bounds specified.
 *
 * @param   this    pointer to an rldisp
 * @param   x       x coordinate of the box
 * @param   y       y coordinate of the box
 * @param   width   width of the box
 * @param   height  height of the box
 * @param   thick   thickness of the box
 * @param   hue     hue of the box
 */
extern void
rldisp_drboxo(rldisp *this, int x, int y, int width, int height, int thick,
    rlhue hue);

/* @brief   Draws an inward empty box directly to an rldisp's frame buffer
 * 
 * The coordinates should be based on the rldisp's frame buffer, not its window
 * size. The thickness of the box moves inward from the bounds specified.
 *
 * @param   this    pointer to an rldisp
 * @param   x       x coordinate of the box
 * @param   y       y coordinate of the box
 * @param   width   width of the box
 * @param   height  height of the box
 * @param   thick   thickness of the box
 * @param   hue     hue of the box
 */
extern void
rldisp_drboxi(rldisp *this, int x, int y, int width, int height, int thick,
    rlhue hue);

/* @brief   Draws a filled-in box directly to an rldisp's frame buffer
 * 
 * The coordinates should be based on the rldisp's frame buffer, not its window
 * size.
 *
 * @param   this    pointer to an rldisp
 * @param   x       x coordinate of the box
 * @param   y       y coordinate of the box
 * @param   width   width of the box
 * @param   height  height of the box
 * @param   hue     hue of the box
 */
extern void
rldisp_drboxf(rldisp *this, int x, int y, int width, int height, rlhue hue);

/* @brief   Finalizes rendering an rldisp for a frame
 *
 * You should end every discrete frame of rendering by calling this function.
 *
 * @param   this    pointer to an rldisp
 */
extern void
rldisp_prsnt(rldisp *this);

/* @brief   Returns the pressed status of an input key for an rldisp
 *
 * @param   this    pointer to an rldisp
 * @param   key     rlkey to check the status of
 *
 * @return  true if the key is pressed, false otherwise
 */
extern bool
rldisp_key(rldisp *this, rlkey key);

/* @brief   Returns the x position of the mouse adjusted to an rldisp's frame
 *
 * If the mouse is outside of the rldisp window, then the coordinate returned
 * is the absolute position on the monitor relative to the top left corner of
 * the window. If the mouse is inside of the rldisp window, then the coordinate
 * returned is relative to the top left corner of the window, then adjusted to
 * the size of the frame buffer.
 *
 * For example, if the window dimensions are 800x600, but the frame buffer
 * dimensions are 640x480, and the mouse is exactly halfway across the window
 * horizontally, then the coordinate returned will be 320.
 *
 * @param   this    pointer to an rldisp
 * 
 * @return  x coordinate of the mouse adjusted to an rldisp's frame buffer
 */
extern int
rldisp_mousx(rldisp *this);

/* @brief   Returns the y position of the mouse adjusted to an rldisp's frame
 *
 * If the mouse is outside of the rldisp window, then the coordinate returned
 * is the absolute position on the monitor relative to the top left corner of
 * the window. If the mouse is inside of the rldisp window, then the coordinate
 * returned is relative to the top left corner of the window, then adjusted to
 * the size of the frame buffer.
 *
 * For example, if the window dimensions are 800x600, but the frame buffer
 * dimensions are 640x480, and the mouse is exactly halfway down the window
 * vertically, then the coordinate returned will be 240.
 *
 * @param   this    pointer to an rldisp
 * 
 * @return  y coordinate of the mouse adjusted to an rldisp's frame buffer
 */
extern int
rldisp_mousy(rldisp *this);

/* @brief   Sets args to x and y pos of the mouse adjusted to an rldisp's frame
 *
 * If the mouse is outside of the rldisp window, then the coords set will be
 * the absolute position on the monitor relative to the top left corner of the
 * window. If the mouse is inside of the rldisp window, then the coords set are
 * relative to the top left corner of the window, then adjusted to the size of
 * the frame buffer.
 *
 * For example, if the window dimensions are 800x600, but the frame buffer
 * dimensions are 640x480, and the mouse is exactly in the center of the window
 * both horizontally and vertically, then the values set will be 320x240.
 *
 * @param   this    pointer to an rldisp
 * @param   mousex  pointer to an int to set to the x mouse coordinate
 * @param   mousey  pointer to an int to set to the y mouse coordinate
 */
extern void
rldisp_mouse(rldisp *this, int *mousex, int *mousey);

/******************************************************************************
rltile function declarations
******************************************************************************/

/* @brief   Returns a pointer to a new rltile
 *
 * @param   glyph   the glyph of the tile
 * @param   fghue   rlhue for the foreground of the tile (the character itself)
 * @param   bghue   rlhue for the background of the tile
 * @param   type    rltype of the tile
 * @param   right   amount the tile's character should be shifted towards the
 *                  right (matters for RL_TILE_EXACT type tiles only)
 * @param   bottom  amount the tile's character should be shifted towards the
 *                  bottom (matters for RL_TILE_EXACT type tiles only)
 * 
 * @return  a pointer to the new rltile
 */
extern rltile *
rltile_init(wchar_t glyph, rlhue fghue, rlhue bghue, rlttype type, float right,
    float bottom);

/* @brief   Returns a pointer to a new rltile with default arguments
 *
 * The default arguments are L'?' for the character, red for the foreground,
 * blue for the background, RL_TILE_CENTER for the type, and 0.0f for both
 * the right and bottom shift.
 *
 * @return  a pointer to the new default tile
 */
extern rltile *
rltile_null(void);

/* @brief   Sets an rltile's character
 *
 * @param   this    pointer to an rltile
 * @param   glyph   the new glyph
 */
extern void
rltile_glyph(rltile *this, wchar_t glyph);

/* @brief   Sets an rltile's foreground color
 *
 * @param   this    pointer to an rltile
 * @param   hue     the new foreground hue
 */
extern void
rltile_fghue(rltile *this, rlhue hue);

/* @brief   Sets an rltile's background color
 *
 * @param   this    pointer to an rltile
 * @param   hue     the new background hue
 */
extern void
rltile_bghue(rltile *this, rlhue hue);

/* @brief   Sets an rltile's type
 *
 * @param   this    pointer to an rltile
 * @param   type    the new type
 */
extern void
rltile_type(rltile *this, rlttype type);

/* @brief   Sets an rltile's right shift value
 *
 * @param   this    pointer to an rltile
 * @param   right   new right shift value
 */
extern void
rltile_right(rltile *this, float right);

/* @brief   Sets an rltile's bottom shift value
 *
 * @param   this    pointer to an rltile
 * @param   bottom  new bottom shift value
 */
extern void
rltile_bottm(rltile *this, float bottom);

/* @brief   Frees the memory allocated for an rltile
 *
 * @param   this    pointer to an rltile
 */
extern void
rltile_free(rltile *this);

/******************************************************************************
rltmap function declarations
******************************************************************************/

/* @brief   Returns a pointer to a new rltmap
 *
 * @param   font    relative path to the font file in the local filesystem,
 *                  supported types are TrueType, Type 1, CFF, OpenType, SFNT,
 *                  X11 PCF, Windows FNT, BDF, PFR and Type 42A
 * @param   csize   pt size for the characters in the map
 * @param   cnum    highest unicode value to support
 * @param   width   width of the map (in # of characters)
 * @param   height  height of the map (in # of characters)
 * @param   offx    horizontal offset of each character in the map
 * @param   offy    vertical offset of each character in the mapA
 *
 * @return  pointer to the new rltmap
 */
extern rltmap *
rltmap_init(const char *font, int csize, int cnum, int width, int height,
    int offx, int offy);

/* @brief   Sets the position of an rltmap relative to the rldisp frame buffer
 *
 * The default position for new rltmaps is 0x0
 *
 * @param   this    pointer to an rltmap
 * @param   x       new x position
 * @param   y       new y position
 */
extern void
rltmap_dpos(rltmap *this, int x, int y);

/* @brief   Scales the size of an rltmap using nearest filter
 *
 * The default scale is rltmap_scale(this, 1.0f). This function sets the scale
 * of the rltmap, so subsequent calls are not additive.
 *
 * @param   this    pointer to an rltmap
 * @param   scale   new scale value
 */
extern void
rltmap_scale(rltmap *this, float scale);

/* @brief   Sets the origin coordinates of an rltmap
 *
 * The origin is used when calculating offsets for rltiles of type
 * RL_TILE_EXACT and when rotating the rltmap.
 *
 * @param   this    pointer to an rltmap
 * @param   xorig   x coordinate of the new origin
 * @param   yorig   y coordinate of the new origin
 */
extern void
rltmap_orign(rltmap *this, int origx, int origy);

/* @brief   Rotates an rltmap clockwise around an internal coordinate
 *
 * The default rotation is rltmap_angle(this, 0, 0, 0.0f). This function sets
 * the rotation of the rltmap, so subsequent calls are not additive.
 *
 * @param   this    pointer to an rltmap
 * @param   rot     degrees to rotate clockwise
 */
extern void
rltmap_angle(rltmap *this, float rot);

/* @brief   Updates an rltmap at coords with tile data
 *
 * @param   this    pointer to an rltmap
 * @param   t       pointer to an rltile to update the rltmap with
 * @param   x       x coordinate of the tile to update (within the rltmap)
 * @param   y       y coordiante of the tile to update (within the rltmap)
 */
extern void
rltmap_tile(rltmap *this, rltile *t, int x, int y);

/* @brief   Updates an rltmap with a *wchar_t, moving towards the right
 *
 * Places a wide string onto the rltmap, starting from the position specified
 * and moving to the right. The characters will wrap around to the next row
 * if the end of the current one is reached. The wchar_t array must be
 * terminated with an L'\0' or this function will segfault.
 *
 * @param   this    pointer to an rltmap
 * @param   wstr    pointer to an array of wchar_t which must end with L'\0'
 * @param   fghue   foreground color to use for each of the rltiles
 * @param   bghue   background color to use for each of the rltiles
 * @param   type    rltiletype to use for each of the tiles
 * @param   x       starting x coordinate (within the rltmap)
 * @param   y       starting y coordinate (within the rltmap)
 */
extern void
rltmap_wstrr(rltmap *this, wchar_t *wstr, rlhue fghue, rlhue bghue,
    rlttype type, int x, int y);

/* @brief   Updates an rltmap with a *wchar_t, moving towards the bottom
 *
 * Places a wide string onto the rltmap, starting from the position specified
 * and moving to the bottom. The characters will wrap around to the next col
 * if the end of the current one is reached. The wchar_t array must be
 * terminated with an L'\0' or this function will segfault.
 *
 * @param   this    pointer to an rltmap
 * @param   wstr    pointer to an array of wchar_t which must end with L'\0'
 * @param   fghue   foreground color to use for each of the rltiles
 * @param   bghue   background color to use for each of the rltiles
 * @param   type    rltiletype to use for each of the tiles
 * @param   x       starting x coordinate (within the rltmap)
 * @param   y       starting y coordinate (within the rltmap)
 */
extern void
rltmap_wstrb(rltmap *this, wchar_t *wstr, rlhue fghue, rlhue bghue,
        rlttype type, int x, int y);

/* @brief   Frees the memory allocated for an rltmap
 *
 * @param   this    pointer to an rltmap
 */
extern void
rltmap_free(rltmap *this);

/* @brief   Returns the x pos of the mouse as a tile coord within an tmap
 *
 * Given a rltmap and the rldisp that it would be drawn to, this function
 * returns the x coordinate of the tile on the rltmap that the mouse is over,
 * or -1 if the mouse is not over where the rltmap would be drawn to.
 *
 * @param   this    pointer to an rltmap
 * @param   disp    pointer to an rldisp that the rltmap will be drawn to
 *
 * @return  The x coordinate of the tile the mouse is over, or -1 if the mouse
 *          is not within the rltmap
 */
extern int
rltmap_mousx(rltmap *this, rldisp *disp);

/* @brief   Returns the y pos of the mouse as a tile coord within an tmap
 *
 * Given a rltmap and the rldisp that it would be drawn to, this function
 * returns the y coordinate of the tile on the rltmap that the mouse is over,
 * or -1 if the mouse is not over where the rltmap would be drawn to.
 *
 * @param   this    pointer to an rltmap
 * @param   disp    pointer to an rldisp that the rltmap will be drawn to
 *
 * @return  The y coordinate of the tile the mouse is over, or -1 if the mouse
 *          is not within the rltmap
 */
extern int
rltmap_mousy(rltmap *this, rldisp *disp);

/* @brief   Sets the args to the coordinates of the mouse within an rltmap
 *
 * Given a rltmap and the rldisp that it would be drawn to, this function
 * sets the int * arguments to the x and y coordinates of the tile that the
 * mouse is currently over, or it sets them both to -1 if the mouse is not
 * over where the rltmap would be drawn to.
 *
 * @param   this    pointer to an rltmap
 * @param   disp    pointer to an rldisp that the rltmap will be drawn to
 * @param   x       pointer to an int to set to the x tile coordinates to
 * @param   y       pointer to an int to set to the y tile coordinates to
 */
extern void
rltmap_mouse(rltmap *this, rldisp *disp, int *x, int *y);

/******************************************************************************
rlhue function declarations
******************************************************************************/

/* @brief   Sets the values of an rlhue
 *
 * @param   this    pointer to an rlhue
 * @param   r       new red value
 * @param   g       new green value
 * @param   b       new blue value
 * @param   a       new alpha value
 */
extern void
rlhue_set(rlhue *this, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/* @brief   Increments the rgba values of an rlhue
 *
 * @param   this    pointer to an rlhue
 * @param   r       red increment value
 * @param   g       green increment value
 * @param   b       blue increment value
 * @param   a       alpha increment value
 */
extern void
rlhue_add(rlhue *this, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/* @brief   Decrements the rgba values of an rlhue
 *
 * @param   this    pointer to an rlhue
 * @param   r       red decrement value
 * @param   g       green decrement value
 * @param   b       blue decrement value
 * @param   a       alpha decrement value
 */
extern void
rlhue_sub(rlhue *this, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

#ifdef __cplusplus
}
#endif

#endif /* RL_DISPLAY_H */
