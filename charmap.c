#include "charmap.h"

/* OpenGL includes */
#include "GL/gl.h"
#include "GL/glut.h"

/* Other includes*/
#include <string.h>

static void find_true_width(Charmap_T* cm)
{
    /* @TODO this function cause a seg fault sometimes */
    memset(cm->trimmed, (cm->glyph_w / 2u), ((VISIBLE_ASCII_HIGH - VISIBLE_ASCII_LOW) + 1u) * 2u);
    /** @TODO verify visible low & high is present in provided charmap */
    for(U8 c = VISIBLE_ASCII_LOW; c < VISIBLE_ASCII_HIGH; ++c)
    {
        uchar real_c = (c - cm->start_ascii);
        U16 row = (real_c / cm->glyphs_per_row);
        U16 col = (real_c % cm->glyphs_per_row);

        U32 cglyph_w = (cm->glyph_w * cm->img->channels);

        U32 h_off = (row * cm->glyph_h) * (cm->img->width * cm->img->channels);
        U32 w_off = (col * cm->glyph_w) * cm->img->channels;

        uchar* loff = cm->img->pixel_map + (h_off + w_off);
        uchar* roff = cm->img->pixel_map + (h_off + (w_off + cglyph_w));

        U8 idx = ((c - VISIBLE_ASCII_LOW) * 2u);

        for(U32 h = (cm->glyph_h - 1u); h > 0u; --h)
        {
            bool rdone = false, ldone = false;
            for(U32 w = 0u; w <= ((cm->glyph_w * cm->img->channels) / 2u); w += cm->img->channels)
            {
                uchar* loffset = loff + ((h * (cm->img->width * cm->img->channels)) + w);
                uchar* roffset = roff + ((h * (cm->img->width * cm->img->channels)) - w);

                /* left march */
                if(!ldone && loffset[3u] > 0x80)
                {
                    cm->trimmed[idx] = MIN(cm->trimmed[idx], (w / cm->img->channels));
                    ldone = true;
                }

                /* right march */
                if(!rdone && roffset[3u] > 0x80)
                {
                    cm->trimmed[idx + 1u] = MIN(cm->trimmed[idx + 1u], (w / cm->img->channels));
                    rdone = true;
                }

                if(rdone && ldone)
                {
                    break;
                }
            }
        }
    }
}

/**
* DRAW_CHAR
* @params cm --> charmap to draw character from
* @params opts --> opts to use for character drawing
* @params c --> character to darw
* @params trim --> whether to draw the trimmed character or not
*/
static void draw_char(AssetDraw_T* draw, uchar c)
{
    Assets_T* a = (Assets_T*)draw->asset;
    Charmap_T* cm = (Charmap_T*)a->asset;
    CharmapDraw_Opts_T* cm_opts = (CharmapDraw_Opts_T*)draw->options.extra;

    if(cm->img && cm->img->pixel_map)
    {
        uchar real_c = (c - cm->start_ascii);

        U16 row = (real_c / cm->glyphs_per_row);
        U16 col = (real_c % cm->glyphs_per_row);

        U32 h_off = (row * cm->glyph_h) * (cm->img->width * cm->img->channels);
        U32 w_off = (col * cm->glyph_w) * cm->img->channels;
        uchar* soff = cm->img->pixel_map + (h_off + w_off);

        U8 trimmed_idx = ((c - VISIBLE_ASCII_LOW) * 2u);

        U8 w_end_sub = (cm_opts->draw_trimmed) ? cm->trimmed[trimmed_idx + 1u] : 0u;
        uchar* offset = soff + (((cm_opts->draw_trimmed) ? cm->trimmed[trimmed_idx] : 0u) * cm->img->channels);


        /* for(uint8_t y = 0u; y < cm->glyph_h; ++y) */
        /* { */
        glRasterPos2i(draw->options.off_x, draw->options.off_y);
        glDrawPixels(cm->glyph_w, cm->glyph_h, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, offset);
        /*     offset += w_off; */
        /* } */
    }
}

/**
* draw_STRING
* @params str --> string to draw
* @params off_x --> X offset onto the screen
* @params off_y --> Y offset onto the screen
* @params scale_x --> (0 - MAX_CHAR_SCALE_FACTOR_HUNDRETHS_X)
* @params scale_y --> (0 - MAX_CHAR_SCALE_FACTOR_HUNDRETHS_Y)
*/
static void draw_string(AssetDraw_T* draw)
{
    CharmapDraw_Opts_T* cm_opts = (CharmapDraw_Opts_T*)draw->options.extra;
    Charmap_T* cm = (Charmap_T*)draw->asset;
    U32 x = draw->options.off_x;
    U32 y = draw->options.off_y;
    for(U16 i = 0u; cm_opts->text[i] != 0; ++i)
    {
        if(cm_opts->text[i] == 10)
        {
            y -= cm->glyph_h;
            x = draw->options.off_x;
        }
        else
        {
            if(cm_opts->text[i] < ((cm->glyphs_per_row * cm->max_rows) + cm->start_ascii) && (cm_opts->text[i] > cm->start_ascii))
            {
                draw_char(draw, cm_opts->text[i]);
                if(cm_opts->draw_trimmed)
                {
                    U8 trimmed_idx = ((cm_opts->text[i] - VISIBLE_ASCII_LOW) * 2u);
                    x += (cm->glyph_w - cm->trimmed[trimmed_idx + 1u]);
                }
                else
                {
                    x += cm->glyph_w;
                }
            }
            else
            {
                x += cm->glyph_w;
            }
        }
    }
}

static void draw_full_charmap(AssetDraw_T* opts)
{
    Assets_T* a = (Assets_T*)opts->asset;
    Charmap_T* cm = (Charmap_T*)a->asset;

    if(cm->img && cm->img->pixel_map)
    {
        glRasterPos2i(opts->options.off_x, opts->options.off_y);
        glDrawPixels(cm->img->width, cm->img->height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, cm->img->pixel_map);
    }
}

static bool charmap_redraw(clock_t ms, void* opts)
{
    (void)ms;
    (void)opts;
    return false;
}

static void charmap_draw(void* opt)
{
    AssetDraw_T* draw = (AssetDraw_T*)opt;
    CharmapDraw_Opts_T* cm_opts = (CharmapDraw_Opts_T*)draw->options.extra;

    if(cm_opts->text[0u] < 0u)
    {
        draw_full_charmap(draw);
    }
    else
    {
        draw_string(draw);
    }
}

void Charmap_Load(const char* fname, U8 start_ascii, U8 max_rows, U8 glyphs_per_row, Scene_T* scene)
{
    Charmap_T* charmap = malloc(sizeof(Charmap_T));
    charmap->start_ascii = start_ascii;
    charmap->glyphs_per_row = glyphs_per_row;
    charmap->max_rows = max_rows;

    /* this doesn't really matter, sine this will not draw a cropped image. However this could be useful in just loading the asset for use. */
    charmap->img = Assets_LoadImage(scene, fname);

    charmap->glyph_w = (charmap->img->width / charmap->glyphs_per_row);
    charmap->glyph_h = (charmap->img->height / charmap->max_rows);

    /* find_true_width(charmap); */

    Assets_AddToScene(scene, (void*)charmap, fname, ASSET_CHARMAP, charmap_redraw, charmap_draw);
}
