#ifndef CHARMAP_H_
#define CHARMAP_H_

#include "asset.h"

#define VISIBLE_ASCII_LOW 33u
#define VISIBLE_ASCII_HIGH 127u

typedef struct {
    U8 start_ascii;
    U8 glyph_h;
    U8 glyph_w;
    U8 glyphs_per_row;
    U8 max_rows;

    Image_T* img;

    bool is_trimmed;
    U8 trimmed[((VISIBLE_ASCII_HIGH - VISIBLE_ASCII_LOW) + 1u) * 2u];
} Charmap_T;

void Charmap_Setup(Scene_T* scene, const char* fname, U8 start_ascii, U8 max_rows, U8 glyphs_per_row);
void Charmap_CalcTrueWidth(Charmap_T* c);
void Charmap_DrawChar(Charmap_T* charmap, uchar c, AssetDraw_Opts_T opts);
void Charmap_DrawString(Charmap_T* charmap, char* str, AssetDraw_Opts_T opts);

#endif // CHARMAP_H_
