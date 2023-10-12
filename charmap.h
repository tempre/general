#ifndef CHARMAP_H_
#define CHARMAP_H_

#include "asset.h"
#include "image.h"

#define VISIBLE_ASCII_LOW 33u
#define VISIBLE_ASCII_HIGH 127u

#define CHARMAP_DEFAULT "bm.png"

typedef struct {
    U8 start_ascii;
    U8 glyph_h;
    U8 glyph_w;
    U8 glyphs_per_row;
    U8 max_rows;

    Image_T* img;

    U8 trimmed[((VISIBLE_ASCII_HIGH - VISIBLE_ASCII_LOW) + 1u) * 2u];
} Charmap_T;

typedef struct {
    bool draw_trimmed;
    const char* text; /* negative number draws whole charmap */
} CharmapDraw_Opts_T;

void Charmap_Load(const char* fname, U8 start_ascii, U8 max_rows, U8 glyphs_per_row, Scene_T* scene);

#endif // CHARMAP_H_
