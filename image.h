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

Image_T* Assets_LoadImage(const char* fname, Scene_T* scene);

#endif // IMAGE_H_
