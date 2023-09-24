/*
* basic charmap splicer
*
* @TODO
*  - true width -
*  1. xMin xMax []
*  2. yMin yMax []
*  - ttf to bitmap -
*  1. make it []
*/

/* Custom includes */
#include "common.h"
#include "asset.h"

/* OpenGL includes */
#include "GL/gl.h"
#include "GL/glut.h"

/* Other includes */
#include <float.h>
#include <string.h>
#include <time.h>

#define VISIBLE_ASCII_LOW 33u
#define VISIBLE_ASCII_HIGH 127u

/* typedef struct { */
/*     const char* file; */
/*     U8 start_ascii; */
/*     U8 glyph_h; */
/*     U8 glyph_w; */
/*     U8 glyphs_per_row; */
/*     U8 max_rows; */

/*     int height; */
/*     int width; */
/*     int channels; */
/*     stbi_uc* pixel_map; */

/*     bool is_trimmed; */
/*     U8 trimmed[((VISIBLE_ASCII_HIGH - VISIBLE_ASCII_LOW) + 1u) * 2u]; */
/* } Charmap_T; */

/* typedef enum { */
/*     TEST_CHARMAP, */
/*     MAX_CHARMAPS, */
/* } Charmaps_E; */

/* #define MAX_CHAR_SCALE_FACTOR_HUNDRETHS_X 00u */
/* #define MAX_CHAR_SCALE_FACTOR_HUNDRETHS_Y 500u */

/* static Charmap_T charmaps[MAX_CHARMAPS] = */
/* { */
/*     [TEST_CHARMAP] = { .file = "bm.png", .start_ascii = 32u, .glyph_h = 30u, .glyph_w = 33u }, */
/* }; */
/* static U8 current_charmap = 0u; */

#define OPENGL_DEFAULT_WIN_WIDTH 800u
#define OPENGL_DEFAULT_WIN_HEIGHT 600u

typedef struct {
    int window_id;
    U16 width;
    U16 height;
    const char* window_name;

    clock_t start_time;

    Scene_T* scene;
} OpenGL_Window_T;

#define CYCLE_DELAY_MS 17 /* ms delay ~60fps */

static OpenGL_Window_T GL_Window = { .window_id = 0u, .height = OPENGL_DEFAULT_WIN_HEIGHT, .width = OPENGL_DEFAULT_WIN_WIDTH, .window_name = "charmap slicer" };

/* static void find_true_width(void) */
/* { */
/*     Charmap_T* cm = &charmaps[current_charmap]; */
/*     if(!cm->is_trimmed) */
/*     { */
/*         memset(cm->trimmed, (cm->glyph_w / 2u), ((VISIBLE_ASCII_HIGH - VISIBLE_ASCII_LOW) + 1u) * 2u); */
/*         /\** @TODO verify visible low & high is present in provided charmap *\/ */
/*         for(U8 c = VISIBLE_ASCII_LOW; c < VISIBLE_ASCII_HIGH; ++c) */
/*         { */
/*             stbi_uc real_c = (c - cm->start_ascii); */
/*             U16 row = (real_c / cm->glyphs_per_row); */
/*             U16 col = (real_c % cm->glyphs_per_row); */

/*             U32 cglyph_w = (cm->glyph_w * cm->channels); */

/*             U32 h_off = (row * cm->glyph_h) * (cm->width * cm->channels); */
/*             U32 w_off = (col * cm->glyph_w) * cm->channels; */

/*             stbi_uc* loff = cm->pixel_map + (h_off + w_off); */
/*             stbi_uc* roff = cm->pixel_map + (h_off + (w_off + cglyph_w)); */

/*             U8 idx = ((c - VISIBLE_ASCII_LOW) * 2u); */

/*             for(U32 h = (cm->glyph_h - 1u); h > 0u; --h) */
/*             { */
/*                 bool rdone = false, ldone = false; */
/*                 for(U32 w = 0u; w <= ((cm->glyph_w * cm->channels) / 2u); w += cm->channels) */
/*                 { */
/*                     stbi_uc* loffset = loff + ((h * (cm->width * cm->channels)) + w); */
/*                     stbi_uc* roffset = roff + ((h * (cm->width * cm->channels)) - w); */

/*                     /\* LEFT *\/ */
/*                     if(!ldone && loffset[3u] > 0x80) */
/*                     { */
/*                         cm->trimmed[idx] = MIN(cm->trimmed[idx], (w / cm->channels)); */
/*                         ldone = true; */
/*                     } */

/*                     if(!rdone && roffset[3u] > 0x80) */
/*                     { */
/*                         cm->trimmed[idx + 1u] = MIN(cm->trimmed[idx + 1u], (w / cm->channels)); */
/*                         rdone = true; */
/*                     } */

/*                     if(rdone && ldone) */
/*                     { */
/*                         break; */
/*                     } */

/*                 } */
/*             } */
/*             cm->is_trimmed = true; */
/*         } */
/*     } */
/* } */

/* /\** */
/* * DRAW_CHAR */
/* * @params c --> character to draw */
/* * @params off_x --> X offset onto the screen */
/* * @params off_y --> Y offset onto the screen */
/* * @params scale_x --> (0 - MAX_CHAR_SCALE_FACTOR_HUNDRETHS_X) */
/* * @params scale_y --> (0 - MAX_CHAR_SCALE_FACTOR_HUNDRETHS_Y) */
/* *\/ */
/* static void draw_char(stbi_uc c, U32 off_x, U32 off_y, U16 scale_x, U16 scale_y, bool trim) */
/* { */
/*     /\* glBegin(GL_POINTS); *\/ */
/*     Charmap_T* cm = &charmaps[current_charmap]; */
/*     stbi_uc real_c = (c - cm->start_ascii); */

/*     U16 row = (real_c / cm->glyphs_per_row); */
/*     U16 col = (real_c % cm->glyphs_per_row); */

/*     U32 h_off = (row * cm->glyph_h) * (cm->width * cm->channels); */
/*     U32 w_off = (col * cm->glyph_w) * cm->channels; */
/*     stbi_uc* soff = cm->pixel_map + (h_off + w_off); */

/*     U8 trimmed_idx = ((c - VISIBLE_ASCII_LOW) * 2u); */

/*     U32 w_end_sub = (trim) ? cm->trimmed[trimmed_idx + 1u] : 0u; */
/*     stbi_uc* offset = soff + (((trim) ? cm->trimmed[trimmed_idx] : 0u) * cm->channels); */

/*     U32 width = (cm->glyph_w - w_end_sub); */
/*     U32 height = cm->glyph_h; */

/*     glRasterPos2i(100, 400); */
/*     glDrawPixels(cm->glyph_w, cm->glyph_h, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, offset); */
/*     /\* for(U32 h = (cm->glyph_h - 1u); h > 0u; --h) *\/ */
/*     /\* { *\/ */
/*     /\*     U32 w_end_sub = (trim) ? cm->trimmed[trimmed_idx + 1u] : 0u; *\/ */
/*     /\*     for(U32 w = (trim) ? cm->trimmed[trimmed_idx] : 0u; w < ((cm->glyph_w - w_end_sub) * cm->channels) ; w += cm->channels) *\/ */
/*     /\*     { *\/ */
/*     /\*         stbi_uc* offset = soff + ((h * (cm->width * cm->channels)) + w); *\/ */
/*     /\*         glColor4f(offset[0] / 255.0f, offset[1] / 255.0f, offset[2] / 255.0f, offset[3] / 255.0f); *\/ */

/*     /\*         /\\* @TODO this is the wrong math, scaling effects xoff & yoff *\\/ *\/ */
/*     /\*         GLint size_x = (LIMIT(scale_x, 0, MAX_CHAR_SCALE_FACTOR_HUNDRETHS_X) / 100.0f); *\/ */
/*     /\*         GLint size_y = (LIMIT(scale_y, 0, MAX_CHAR_SCALE_FACTOR_HUNDRETHS_Y) / 100.0f); *\/ */

/*     /\*         GLint start_x = off_x + (w / cm->channels); *\/ */
/*     /\*         GLint start_y = off_y + (cm->height - h); *\/ */

/*     /\*         glDrawPixels((trim) ? cm->trimmed[trimmed_idx] : 0u, (cm->glyph_h - 1u) * cm->width, ) *\/ */
/*     /\*         glVertex2f(start_x * size_x, start_y * size_y); *\/ */
/*     /\*         /\\* glRecti(start_x, start_y, start_x * size_x, start_y * size_y); *\\/ *\/ */
/*     /\*     } *\/ */
/*     /\* } *\/ */
/*     /\* glEnd(); *\/ */
/* } */

/* /\** */
/* * DRAW_STRING */
/* * @params str --> string to draw */
/* * @params off_x --> X offset onto the screen */
/* * @params off_y --> Y offset onto the screen */
/* * @params scale_x --> (0 - MAX_CHAR_SCALE_FACTOR_HUNDRETHS_X) */
/* * @params scale_y --> (0 - MAX_CHAR_SCALE_FACTOR_HUNDRETHS_Y) */
/* *\/ */
/* static void draw_string(char* str, U32 off_x, U32 off_y, U16 scale_x, U8 scale_y) */
/* { */
/*     Charmap_T* cm = &charmaps[current_charmap]; */
/*     U32 x = off_x; */
/*     U32 y = off_y; */
/*     for(U16 i = 0u; str[i] != 0; ++i) */
/*     { */
/*         if(str[i] == 10) */
/*         { */
/*             y -= cm->glyph_h; */
/*             x = off_x; */
/*         } */
/*         else */
/*         { */
/*             if(((stbi_uc)str[i]) < ((cm->glyphs_per_row * cm->max_rows) + cm->start_ascii) && ((stbi_uc)str[i] > cm->start_ascii)) */
/*             { */
/*                 draw_char((stbi_uc)str[i], x, y, scale_x, scale_y, false); */
/*             } */
/*             x += cm->glyph_w; */
/*         } */
/*     } */
/* } */

/* static void draw_tstring(char* str, U32 off_x, U32 off_y, U16 scale_x, U16 scale_y) */
/* { */
/*     Charmap_T* cm = &charmaps[current_charmap]; */
/*     U32 x = off_x; */
/*     U32 y = off_y; */
/*     for(U16 i = 0u; str[i] != 0; ++i) */
/*     { */
/*         if(str[i] == 10) */
/*         { */
/*             y -= cm->glyph_h; */
/*             x = off_x; */
/*         } */
/*         else */
/*         { */
/*             if(((stbi_uc)str[i]) < ((cm->glyphs_per_row * cm->max_rows) + cm->start_ascii) && ((stbi_uc)str[i] > cm->start_ascii)) */
/*             { */
/*                 U8 trimmed_idx = ((str[i] - VISIBLE_ASCII_LOW) * 2u); */
/*                 draw_char((stbi_uc)str[i], x, y, scale_x, scale_y, true); */
/*                 x += (cm->glyph_w - cm->trimmed[trimmed_idx + 1u]); */
/*             } */
/*             else */
/*             { */
/*                 x += cm->glyph_w; */
/*             } */
/*         } */
/*     } */
/* } */


static void opengl_loop(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Assets_FlipDrawQueue(GL_Window.scene);
    Assets_ClearDrawQueue(GL_Window.scene);

    Assets_Draw(ASSET_KIKI_PNG, GL_Window.scene, (AssetDraw_Opts_T){ .off_x = 100, .off_y = 200});
    Assets_Draw(ASSET_CAT_GIF, GL_Window.scene, (AssetDraw_Opts_T){ .off_x = 900, .off_y = 500, .gif_speed = GIF_PLAYBACK_SPD_1X});

    Assets_Draw(ASSET_GIPHY_GIF, GL_Window.scene, (AssetDraw_Opts_T){ .off_x = 100, .off_y = 500, .gif_speed = GIF_PLAYBACK_SPD_1X});
    Assets_Draw(ASSET_GIPHY_GIF, GL_Window.scene, (AssetDraw_Opts_T){ .off_x = 501, .off_y = 500, .gif_speed = GIF_PLAYBACK_SPD_3X});
    Assets_Draw(ASSET_GIPHY_GIF, GL_Window.scene, (AssetDraw_Opts_T){ .off_x = 100, .off_y = 200, .gif_speed = GIF_PLAYBACK_SPD_HALF});
    Assets_Draw(ASSET_GIPHY_GIF, GL_Window.scene, (AssetDraw_Opts_T){ .off_x = 501, .off_y = 200, .gif_speed = GIF_PLAYBACK_SPD_EIGHTH});

    Assets_Draw(ASSET_PENG_GIF, GL_Window.scene, (AssetDraw_Opts_T){ .off_x = 100, .off_y = 200, .gif_speed = GIF_PLAYBACK_SPD_1X});

    glutSwapBuffers();
    glFlush();
}

static void opengl_resize(U16 width, U16 height)
{
    GL_Window.width = width;
    GL_Window.height = height;

    /* reset projections */
    glViewport(0, 0, GL_Window.width, GL_Window.height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, GL_Window.width, 0.0, GL_Window.height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    opengl_loop();
}

static void opengl_idle(void)
{
    int clk = clock();
    int ms = (clk - GL_Window.start_time) * 1000 / CLOCKS_PER_SEC;

    if(ms >= CYCLE_DELAY_MS)
    {
        bool redraw = Assets_PromptRedraw(GL_Window.scene, ms);

        if(redraw)
        {
            glutPostRedisplay();
        }

        /* reset start clk */
        GL_Window.start_time = clk;
    }
}

static void opengl_init(int argc, char** argv)
{
    GL_Window.scene = Assets_SetupScene();

    Assets_LoadFile(ASSET_CAT_GIF, ASSET_GIF, GL_Window.scene);
    Assets_LoadFile(ASSET_GIPHY_GIF, ASSET_GIF, GL_Window.scene);
    Assets_LoadFile(ASSET_PENG_GIF, ASSET_GIF, GL_Window.scene);
    Assets_LoadFile(ASSET_KIKI_PNG, ASSET_IMAGE, GL_Window.scene);

    GL_Window.start_time = clock();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(GL_Window.width, GL_Window.height);
    glutInitWindowPosition(0, 0);
    GL_Window.window_id = glutCreateWindow(GL_Window.window_name);

    /* projection setup */
    glViewport(0, 0, GL_Window.width, GL_Window.height);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, GL_Window.width, 0.0, GL_Window.height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glutDisplayFunc(opengl_loop);
    glutReshapeFunc(opengl_resize);
    glutIdleFunc(opengl_idle);
    glutMainLoop();
}

/* static void load_charmaps(void) */
/* { */
/*     for(U8 i = 0u; i < MAX_CHARMAPS; ++i) */
/*     { */
/*         Charmap_T* temp = &charmaps [current_charmap]; */
/*         stbi_set_flip_vertically_on_load(true); */
/*         temp->pixel_map = stbi_load(temp->file, &temp->width, &temp->height, &temp->channels, STBI_rgb_alpha); */

/*         if(temp->pixel_map == NULL) */
/*         { */
/*             AX_DEBUG("failed to load %s", temp->file); */
/*             continue; */
/*         } */

/*         temp->glyphs_per_row = ((float)temp->width / (float)temp->glyph_w); */
/*         temp->max_rows = ((float)temp->height / (float)temp->glyph_h); */
/*     } */
/* } */

int main(int argc, char** argv)
{
    /* load_charmaps(); */
    /* find_true_width(); */ /* SEG FAULT SOMEWHERE HERE */
    opengl_init(argc, argv);
    return 0;
}
