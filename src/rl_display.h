#ifndef RL_DISPLAY_H
#define RL_DISPLAY_H

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
 * @param   ww  width of the window
 * @param   wh  height of the window
 * @param   fw  width of the frame buffer
 * @param   fh  height of the frame buffer
 * @param   n   name of the window
 * @param   f   whether the window is fullscreen
 *
 * @return  a pointer to the new rldisp
 */
extern rldisp *
rldisp_init(int ww, int wh, int fw, int fh, char *n, bool f);

/* @brief   Sets the rldisp to be either fullscreen or windowed.
 *
 * Calling this function will cause the window to be recreated.
 *
 * @param   this    pointer to the rldisp
 * @param   f       whether the window should be set to fullscreen or windowed
 */
extern void
rldisp_fscrn(rldisp *this, bool f);

/* @brief   Sets the rldisp window size
 *
 * Calling this function will cause the window to be recreated. If the window
 * is fullscreen and the dimensions passed are not a valid fullscreen mode,
 * the largest valid mode will be selected.
 *
 * @param   this    pointer to the rldisp
 * @param   w       new width of the window
 * @param   h       new height of the window
 */
extern void
rldisp_rsize(rldisp *this, int w, int h);

/* @brief   Sets the rldisp window name
 *
 * @param   this    pointer to the rldisp
 * @param   n       new window name
 */
extern void
rldisp_rname(rldisp *this, const char *n);

/* @brief   Sets whether or not the rldisp window should enable vertical sync
 *
 * @param   this    pointer to the rldisp
 * @param   e       whether vertical sync is enabled
 */
extern void
rldisp_vsync(rldisp *this, bool e);

/* @brief   Sets whther or not the rldisp window should hide the mouse cursor
 *
 * @param   this    pointer to the rldisp
 * @param   v       whether the mouse cursor is visible
 */
extern void
rldisp_shwcur(rldisp *this, bool v);

/* @brief   Sets the fps limit of the rldisp window (or 0 to disable)
 *
 * The default value for an rldisp is 0 (disabled).
 *
 * @param   this    pointer to the rldisp
 * @param   l       new fps limit
 */
extern void
rldisp_fpslim(rldisp *this, int l);

/* @brief   Frees the memory allocated for an rldisp
 *
 * @param   this    pointer to the rldisp
 */
extern void
rldisp_free(rldisp *this);

/* @brief   Returns the status of the rldisp window
 *
 * @param   this    pointer to the rldisp
 *
 * @return  true if the window is open, false otherwise
 */
extern bool
rldisp_status(rldisp *this);

/* @brief   Flushes the input event queue for the rldisp window
 *
 * This should be called between rldisp_clr(1) and rldisp_prsnt(1) every
 * frame, so that rldisp_key(2) returns the correct value.
 *
 * @param   this    pointer to the rldisp
 */
extern void
rldisp_evtflsh(rldisp *this);

/* @brief   Clears the rldisp window
 *
 * Clears the rldisp window to the color set by rldisp_clrhue(2). You should
 * begin every discrete frame by calling this function.
 *
 * @param   this    pointer to the rldisp
 */
extern void
rldisp_clr(rldisp *this);

/* @brief   Sets the clear color for the rldisp window
 *
 * @param   h   rlhue that the clear color is set to
 */
extern void
rldisp_clrhue(rldisp *this, rlhue h);

/* @brief   Draws a rltmap into the rldisp's frame buffer
 *
 * @param   this    pointer to the rldisp
 * @param   t       pointer to the rltmap
 */
extern void
rldisp_drtmap(rldisp *this, rltmap *t);

/* @brief   Finalizes rendering the rldisp for a frame
 *
 * You should end every discrete frame of rendering by calling this function.
 *
 * @param   this    pointer to the rldisp
 */
extern void
rldisp_prsnt(rldisp *this);

/* @brief   Returns the pressed status of an input key for an rldisp
 *
 * @param   this    pointer to the rldisp
 * @param   k       rlkey to check the status of
 *
 * @return  true if the key is pressed, false otherwise
 */
extern bool
rldisp_key(rldisp *this, rlkey k);

/* @brief   Returns the x position of the mouse adjusted to the frame buffer
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
 * @param   this    pointer to the rldisp
 * 
 * @return  x coordinate of the mouse adjusted to the frame buffer
 */
extern int
rldisp_mousx(rldisp *this);

/* @brief   Returns the y position of the mouse adjusted to the frame buffer
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
 * @param   this    pointer to the rldisp
 * 
 * @return  y coordinate of the mouse adjusted to the frame buffer
 */
extern int
rldisp_mousy(rldisp *this);

/* @brief   Sets the args to the x and y pos of the mouse adjusted to the frame
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
 * @param   this    pointer to the rldisp
 * @param   x       int pointer to set to the x mouse coordinate
 * @param   y       int pointer to set to the y mouse coordinate
 */
extern void
rldisp_mouse(rldisp *this, int *x, int *y);

/******************************************************************************
rltile function declarations
******************************************************************************/

/* @brief   Returns a pointer to a new rltile
 *
 * @param   c   the character of the tile
 * @param   fg  rlhue for the foreground of the tile (the character itself)
 * @param   bg  rlhue for the background of the tile
 * @param   t   rltype of the tile
 * @param   r   amount the tile's character should be shifted towards the
 *              right (matters for RL_TILE_EXACT type tiles only)
 * @param   b   amount the tile's character should be shifted towards the
 *              bottom (matters for RL_TILE_EXACT type tiles only)
 * 
 * @return  a pointer to the new rltile
 */
extern rltile *
rltile_init(wchar_t c, rlhue fg, rlhue bg, rlttype t, float r, float b);

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

/* @brief   Sets the rltile's character
 *
 * @param   this    pointer to the rltile
 * @param   c       the new character
 */
extern void
rltile_chr(rltile *this, wchar_t c);

/* @brief   Sets the rltile's foreground color
 *
 * @param   this    pointer to the rltile
 * @param   h       the new foreground color
 */
extern void
rltile_fghue(rltile *this, rlhue h);

/* @brief   Sets the rltile's background color
 *
 * @param   this    pointer to the rltile
 * @param   h       the new background color
 */
extern void
rltile_bghue(rltile *this, rlhue h);

/* @brief   Sets the rltile's type
 *
 * @param   this    pointer to the rltile
 * @param   t       the new rltiletype
 */
extern void
rltile_type(rltile *this, rlttype t);

/* @brief   Sets the rltile's right shift value
 *
 * @param   this    pointer to the rltile
 * @param   r       new right shift value
 */
extern void
rltile_right(rltile *this, float r);

/* @brief   Sets the rltile's bottom shift value
 *
 * @param   this    pointer to the rltile
 * @param   r       new bottom shift value
 */
extern void
rltile_bottm(rltile *this, float b);

/* @brief   Frees the memory allocated for an rltile
 *
 * @param   this    pointer to the rltile
 */
extern void
rltile_free(rltile *this);

/******************************************************************************
rltmap function declarations
******************************************************************************/

/* @brief   Returns a pointer to a new rltmap
 *
 * @param   f       relative path to the font file in the local filesystem,
 *                  supported types are TrueType, Type 1, CFF, OpenType, SFNT,
 *                  X11 PCF, Windows FNT, BDF, PFR and Type 42A
 * @param   csz     pt size for the characters in the map
 * @param   w       width of the map (in # of characters)
 * @param   h       height of the map (in # of characters)
 * @param   offx    horizontal offset of each character in the map
 * @param   offy    vertical offset of each character in the mapA
 *
 * @return  pointer to the new rltmap
 */
extern rltmap *
rltmap_init(const char *f, int csz, int w, int h, int offx, int offy);

/* @brief   Sets the position of the rltmap relative to the rldisp frame buffer
 *
 * The default position for new rltmaps is 0.0x0.0
 *
 * @param   this    pointer to the rltmap
 * @param   x       x position (can be real)
 * @param   y       y position (can be real)
 */
extern void
rltmap_pos(rltmap *this, float x, float y);

/* @brief   Updates the rltmap at coords with tile data
 *
 * @param   this    pointer to the rltmap
 * @param   t       pointer to the rltile to update the rltmap with
 * @param   x       x coordinate of the tile to update (within the rltmap)
 * @param   y       y coordiante of the tile to update (within the rltmap)
 */
extern void
rltmap_tile(rltmap *this, rltile *t, int x, int y);

/* @brief   Updates the rltmap with a *wchar_t, moving towards the right
 *
 * Places a wide string onto the rltmap, starting from the position specified
 * and moving to the right. The characters will wrap around to the next row
 * if the end of the current one is reached. The wchar_t array must be
 * terminated with an L'\0' or this function will segfault.
 *
 * @param   this    pointer to the rltmap
 * @param   s       pointer to an array of wchar_t which must end with L'\0'
 * @param   fg      foreground color to use for each of the rltiles
 * @param   bg      background color to use for each of the rltiles
 * @param   t       rltiletype to use for each of the tiles
 * @param   x       starting x coordinate (within the rltmap)
 * @param   y       starting y coordinate (within the rltmap)
 */
extern void
rltmap_wstrr(rltmap *this, wchar_t *s, rlhue fg, rlhue bg, rlttype t, int x,
    int y);

/* @brief   Updates the rltmap with a *wchar_t, moving towards the bottom
 *
 * Places a wide string onto the rltmap, starting from the position specified
 * and moving to the bottom. The characters will wrap around to the next col
 * if the end of the current one is reached. The wchar_t array must be
 * terminated with an L'\0' or this function will segfault.
 *
 * @param   this    pointer to the rltmap
 * @param   s       pointer to an array of wchar_t which must end with L'\0'
 * @param   fg      foreground color to use for each of the rltiles
 * @param   bg      background color to use for each of the rltiles
 * @param   t       rltiletype to use for each of the tiles
 * @param   x       starting x coordinate (within the rltmap)
 * @param   y       starting y coordinate (within the rltmap)
 */
extern void
rltmap_wstrb(rltmap *this, wchar_t *s, rlhue fg, rlhue bg, rlttype t, int x,
    int y);

/* @brief   Frees the memory allocated for an rltmap
 *
 * @param   this    pointer to the rltmap
 */
extern void
rltmap_free(rltmap *this);

/* @brief   Returns the x pos of the mouse as a tile coord within the tmap
 *
 * Given a rltmap and the rldisp that it would be drawn to, this function
 * returns the x coordinate of the tile on the rltmap that the mouse is over,
 * or -1 if the mouse is not over where the rltmap would be drawn to.
 *
 * @param   this    pointer to the rltmap
 * @param   d       pointer to the rldisp that the rltmap will be drawn to
 *
 * @return  The x coordinate of the tile the mouse is over, or -1 if the mouse
 *          is not within the rltmap
 */
extern int
rltmap_mousx(rltmap *this, rldisp *d);

/* @brief   Returns the y pos of the mouse as a tile coord within the tmap
 *
 * Given a rltmap and the rldisp that it would be drawn to, this function
 * returns the y coordinate of the tile on the rltmap that the mouse is over,
 * or -1 if the mouse is not over where the rltmap would be drawn to.
 *
 * @param   this    pointer to the rltmap
 * @param   d       pointer to the rldisp that the rltmap will be drawn to
 *
 * @return  The y coordinate of the tile the mouse is over, or -1 if the mouse
 *          is not within the rltmap
 */
extern int
rltmap_mousy(rltmap *this, rldisp *d);

/* @brief   Sets the args to the coordinates of the mouse within the rltmap
 *
 * Given a rltmap and the rldisp that it would be drawn to, this function
 * sets the int * arguments to the x and y coordinates of the tile that the
 * mouse is currently over, or it sets them both to -1 if the mouse is not
 * over where the rltmap would be drawn to.
 *
 * @param   this    pointer to the rltmap
 * @param   d       pointer to the rldisp that the rltmap will be drawn to
 * @param   x       int pointer to set to the x rltmap tile coordinates to
 * @param   y       int pointer to set to the y rltmap tile coordinates to
 */
extern void
rltmap_mouse(rltmap *this, rldisp *d, int *x, int *y);

#endif /* RL_DISPLAY_H */
