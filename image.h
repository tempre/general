#ifndef IMAGE_H_
#define IMAGE_H_

#include "common.h"
#include "asset.h"

typedef struct {
    int height;
    int width;
    int channels;
    uchar* pixel_map;
} Image_T;

Image_T* Assets_LoadImage(Scene_T* scene, const char* fname);
void Assets_DrawImage(void* opt);
bool Assets_RedrawImage(clock_t ms, void* opts);

#endif // IMAGE_H_
