#include "glyph_tile.h"

#include <stdlib.h>

struct glyph_tile *
glyph_tile_create(wchar_t glyph, uint8_t fg[4], uint8_t bg[4],
    enum glyph_tile_type type, float right, float bottom)
{
    struct glyph_tile *this = NULL;

    if (!(this = malloc(sizeof(struct glyph_tile))))
        return NULL;

    this->glyph = glyph;
    
    for (int i = 0; i < 4; i++)
    {
        this->fg[i] = fg[i];
        this->bg[i] = bg[i];
    }

    this->type = type;
    this->right = right;
    this->bottom = bottom;

    return this;
}

struct glyph_tile *
glyph_tile_default(void)
{
    uint8_t fg[4] = {255, 0, 0, 255};
    uint8_t bg[4] = {0, 0, 255, 255};

    return glyph_tile_create(L'?', fg, bg, GLYPH_TILE_TYPE_CENTER, 0.0f, 0.0f);
}

void
glyph_tile_cleanup(struct glyph_tile *this)
{
    if (!this)
        return;

    free(this);
}
