#include "display.h"

#include <stdlib.h>
#include <string.h>
#include <SFML/System.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>

struct display
{
    struct {
        char *title;
        unsigned width;
        unsigned height;
        sfColor clear_color;
        sfRenderWindow *handle;
    } window;
};

static char *
strdup(const char *s)
{
    char *d = NULL;
    size_t len = strlen(s);

    if (!s || !(d = malloc(len + 1)))
        return NULL;

    d[len] = '\0';
    strcpy(d, s);
    return d;
}

static void
display_resized(struct display *this, unsigned width, unsigned height)
{
    if (!this)
        return;

    this->window.width = width;
    this->window.height = height;
}

struct display *
display_create(unsigned width, unsigned height, char *title, bool fscreen)
{
    struct display *this = NULL;
    sfVideoMode mode = {width, height, 32};
    sfUint32 style = (fscreen) ? sfFullscreen : sfClose | sfTitlebar;

    if (!(this = malloc(sizeof(struct display))))
        return NULL;

    if (!(this->window.title = strdup(title)))
    {
        free(this);
        return NULL;
    }

    if (!(this->window.handle = sfRenderWindow_create(mode, title, style,
        NULL)))
    {
        free(this->window.title);
        free(this);

        return NULL;
    }

    this->window.clear_color = sfBlack;

    return this;
}

void
display_fscreen(struct display *this, bool fscreen)
{
    sfUint32 style = (fscreen) ? sfFullscreen : sfDefaultStyle;
    sfVideoMode mode = {this->window.width, this->window.height, 32};

    if (!this || !this->window.handle)
        return;

    sfRenderWindow_destroy(this->window.handle);

    this->window.handle = sfRenderWindow_create(mode, this->window.title,
        style, NULL);
}

void
display_resize(struct display *this, unsigned width, unsigned height)
{
    sfVector2u size = {width, height};

    if (!this || !this->window.handle)
        return;

    this->window.width = width;
    this->window.height = height;
    sfRenderWindow_setSize(this->window.handle, size);
}

void
display_rename(struct display *this, const char *title)
{
    if (!this || !this->window.title)
        return;

    free(this->window.title);

    this->window.title = strdup(title);

    sfRenderWindow_setTitle(this->window.handle, title);
}

extern void
display_vsync(struct display *this, bool enabled)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setVerticalSyncEnabled(this->window.handle, enabled);
}

extern void
display_cursor(struct display *this, bool visible)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setMouseCursorVisible(this->window.handle, visible);
}

extern void
display_framerate(struct display *this, unsigned limit)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_setFramerateLimit(this->window.handle, limit);
}

void
display_cleanup(struct display *this)
{
    if (!this)
        return;

    if (this->window.title)
        free(this->window.title);

    if (this->window.handle)
        sfRenderWindow_destroy(this->window.handle);


    free(this);
}

extern bool
display_status(struct display *this)
{
    if (!this || !this->window.handle)
        return false;

    return sfRenderWindow_isOpen(this->window.handle);
}

extern void
display_events_flush(struct display *this)
{
    static sfEvent evt;

    if (!this || !this->window.handle)
        return;

    while (sfRenderWindow_pollEvent(this->window.handle, &evt))
    {
        switch (evt.type)
        {
            case sfEvtClosed:
                sfRenderWindow_close(this->window.handle);
                break;
            case sfEvtResized:
                display_resized(this, evt.size.width, evt.size.height);
                break;
            default:
                break;
        }
    }
}

extern void
display_clear(struct display *this)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_clear(this->window.handle, this->window.clear_color);
}

extern void
display_clear_color(struct display *this, unsigned char rgba[4])
{
    if (!this)
        return;

    this->window.clear_color = (sfColor){rgba[0], rgba[1], rgba[2], rgba[3]};
}

extern void
display_present(struct display *this)
{
    if (!this || !this->window.handle)
        return;

    sfRenderWindow_display(this->window.handle);
}

extern bool
display_key_pressed(struct display *this, enum display_key key)
{
    if (!this || key < 0 || key >= DISPLAY_KEY_MAXIMUM)
        return false;

    // *this is unused in the SFML renderer implementation, but the interface
    // requires it to be passed for compatibility with other backends
    (void)this;

    switch (key)
    {
    case DISPLAY_KEY_A:
        return sfKeyboard_isKeyPressed(sfKeyA);
    case DISPLAY_KEY_B:
        return sfKeyboard_isKeyPressed(sfKeyB);
    case DISPLAY_KEY_C:
        return sfKeyboard_isKeyPressed(sfKeyC);
    case DISPLAY_KEY_D:
        return sfKeyboard_isKeyPressed(sfKeyD);
    case DISPLAY_KEY_E:
        return sfKeyboard_isKeyPressed(sfKeyE);
    case DISPLAY_KEY_F:
        return sfKeyboard_isKeyPressed(sfKeyF);
    case DISPLAY_KEY_G:
        return sfKeyboard_isKeyPressed(sfKeyG);
    case DISPLAY_KEY_H:
        return sfKeyboard_isKeyPressed(sfKeyH);
    case DISPLAY_KEY_I:
        return sfKeyboard_isKeyPressed(sfKeyI);
    case DISPLAY_KEY_J:
        return sfKeyboard_isKeyPressed(sfKeyJ);
    case DISPLAY_KEY_K:
        return sfKeyboard_isKeyPressed(sfKeyK);
    case DISPLAY_KEY_L:
        return sfKeyboard_isKeyPressed(sfKeyL);
    case DISPLAY_KEY_M:
        return sfKeyboard_isKeyPressed(sfKeyM);
    case DISPLAY_KEY_N:
        return sfKeyboard_isKeyPressed(sfKeyN);
    case DISPLAY_KEY_O:
        return sfKeyboard_isKeyPressed(sfKeyO);
    case DISPLAY_KEY_P:
        return sfKeyboard_isKeyPressed(sfKeyP);
    case DISPLAY_KEY_Q:
        return sfKeyboard_isKeyPressed(sfKeyQ);
    case DISPLAY_KEY_R:
        return sfKeyboard_isKeyPressed(sfKeyR);
    case DISPLAY_KEY_S:
        return sfKeyboard_isKeyPressed(sfKeyS);
    case DISPLAY_KEY_T:
        return sfKeyboard_isKeyPressed(sfKeyT);
    case DISPLAY_KEY_U:
        return sfKeyboard_isKeyPressed(sfKeyU);
    case DISPLAY_KEY_V:
        return sfKeyboard_isKeyPressed(sfKeyV);
    case DISPLAY_KEY_W:
        return sfKeyboard_isKeyPressed(sfKeyW);
    case DISPLAY_KEY_X:
        return sfKeyboard_isKeyPressed(sfKeyX);
    case DISPLAY_KEY_Y:
        return sfKeyboard_isKeyPressed(sfKeyY);
    case DISPLAY_KEY_Z:
        return sfKeyboard_isKeyPressed(sfKeyZ);
    case DISPLAY_KEY_0:
        return sfKeyboard_isKeyPressed(sfKeyNum0) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad0);
    case DISPLAY_KEY_1:
        return sfKeyboard_isKeyPressed(sfKeyNum1) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad1);
    case DISPLAY_KEY_2:
        return sfKeyboard_isKeyPressed(sfKeyNum2) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad2);
    case DISPLAY_KEY_3:
        return sfKeyboard_isKeyPressed(sfKeyNum3) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad3);
    case DISPLAY_KEY_4:
        return sfKeyboard_isKeyPressed(sfKeyNum4) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad4);
    case DISPLAY_KEY_5:
        return sfKeyboard_isKeyPressed(sfKeyNum5) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad5);
    case DISPLAY_KEY_6:
        return sfKeyboard_isKeyPressed(sfKeyNum6) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad6);
    case DISPLAY_KEY_7:
        return sfKeyboard_isKeyPressed(sfKeyNum7) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad7);
    case DISPLAY_KEY_8:
        return sfKeyboard_isKeyPressed(sfKeyNum8) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad8);
    case DISPLAY_KEY_9:
        return sfKeyboard_isKeyPressed(sfKeyNum9) ||
            sfKeyboard_isKeyPressed(sfKeyNumpad9);
    case DISPLAY_KEY_ESCAPE:
        return sfKeyboard_isKeyPressed(sfKeyEscape);
    case DISPLAY_KEY_CONTROL:
        return sfKeyboard_isKeyPressed(sfKeyLControl) ||
            sfKeyboard_isKeyPressed(sfKeyRControl);
    case DISPLAY_KEY_SHIFT:
        return sfKeyboard_isKeyPressed(sfKeyLShift) ||
            sfKeyboard_isKeyPressed(sfKeyRShift);
    case DISPLAY_KEY_ALT:
        return sfKeyboard_isKeyPressed(sfKeyLAlt) ||
            sfKeyboard_isKeyPressed(sfKeyRAlt);
    case DISPLAY_KEY_SYSTEM:
        return sfKeyboard_isKeyPressed(sfKeyLSystem) ||
            sfKeyboard_isKeyPressed(sfKeyRSystem);
    case DISPLAY_KEY_SEMICOLON:
        return sfKeyboard_isKeyPressed(sfKeySemiColon);
    case DISPLAY_KEY_COMMA:
        return sfKeyboard_isKeyPressed(sfKeyComma);
    case DISPLAY_KEY_PERIOD:
        return sfKeyboard_isKeyPressed(sfKeyPeriod);
    case DISPLAY_KEY_QUOTE:
        return sfKeyboard_isKeyPressed(sfKeyQuote);
    case DISPLAY_KEY_SLASH:
        return sfKeyboard_isKeyPressed(sfKeySlash);
    case DISPLAY_KEY_TILDE:
        return sfKeyboard_isKeyPressed(sfKeyTilde);
    case DISPLAY_KEY_SPACE:
        return sfKeyboard_isKeyPressed(sfKeySpace);
    case DISPLAY_KEY_ENTER:
        return sfKeyboard_isKeyPressed(sfKeyReturn);
    case DISPLAY_KEY_BACKSPACE:
        return sfKeyboard_isKeyPressed(sfKeyBack);
    case DISPLAY_KEY_TAB:
        return sfKeyboard_isKeyPressed(sfKeyTab);
    case DISPLAY_KEY_UP:
        return sfKeyboard_isKeyPressed(sfKeyUp);
    case DISPLAY_KEY_DOWN:
        return sfKeyboard_isKeyPressed(sfKeyDown);
    case DISPLAY_KEY_LEFT:
        return sfKeyboard_isKeyPressed(sfKeyLeft);
    case DISPLAY_KEY_RIGHT:
        return sfKeyboard_isKeyPressed(sfKeyRight);
    default:
        return false;
    }
}
