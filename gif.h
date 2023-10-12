#ifndef GIF_H_
#define GIF_H_

#include "common.h"
#include "asset.h"

typedef struct {
    uchar* data;
    uchar* frame_buff;

    int* delays; /* ms */

    int height;
    int width;
    int channels;
    int frames;
    int file_size;
} GIF_T;

typedef struct {
    U16 speed;
    U16 gif_frame;
} GIF_DrawOpts_T;

typedef enum {
    GIF_PLAYBACK_SPD_EIGHTH = 8u,
    GIF_PLAYBACK_SPD_FOURTH = 4u,
    GIF_PLAYBACK_SPD_HALF   = 2u,
    GIF_PLAYBACK_SPD_1X     = 100u,
    GIF_PLAYBACK_SPD_2X     = 200u,
    GIF_PLAYBACK_SPD_3X     = 300u,
} GIF_PLAYBACK_SPEED_e;


GIF_T* Assets_LoadGIF(const char* fname, Scene_T* scene);

#endif // GIF_H_
