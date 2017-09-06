#ifndef GLYPH_TILE_H
#define GLYPH_TILE_H

#include <wchar.h>
#include <stdint.h>

enum glyph_tile_type {
    GLYPH_TILE_TYPE_TEXT,
    GLYPH_TILE_TYPE_EXACT,
    GLYPH_TILE_TYPE_FLOOR,
    GLYPH_TILE_TYPE_CENTER
};

struct glyph_tile {
    float right;
    float bottom;
    wchar_t glyph;
    uint8_t fg[4];
    uint8_t bg[4];
    enum glyph_tile_type type;
};

extern struct glyph_tile *
glyph_tile_create(wchar_t glyph, uint8_t fg[4], uint8_t bg[4],
    enum glyph_tile_type type, float right, float bottom);

extern struct glyph_tile *
glyph_tile_default(void);

extern void
glyph_tile_cleanup(struct glyph_tile *this);

#endif /* GLYPH_TILE_H */
