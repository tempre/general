#include "charmap.h"

static void charmap_redraw(clock_t ms, void* opts)
{

}

static void charmap_draw(clock_t ms, void* opts)
{

}

void Charmap_Setup(Scene_T* scene, const char* fname, U8 start_ascii, U8 max_rows, U8 glyphs_per_row)
{
    Charmap_T* charmap = malloc(sizeof(Charmap_T));
    charmap->start_ascii = start_ascii;
    charmap->glyphs_per_row = glyphs_per_row;
    charmap->max_rows = max_rows;

    /* this doesn't really matter, sine this will not draw a cropped image. However this could be useful in just loading the asset for use. */
    charmap->img = (Image_T*)Assets_LoadFile(fname, ASSET_IMAGE, scene);
}
