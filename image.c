#include "image.h"

/* OpenGL includes */
#include "GL/gl.h"
#include "GL/glut.h"

Image_T* Assets_LoadImage(Scene_T* scene, const char* fname)
{
    Image_T* img = malloc(sizeof(Image_T));
    stbi_set_flip_vertically_on_load(true);
    img->pixel_map = stbi_load(fname, &img->width, &img->height, &img->channels, STBI_rgb_alpha);

    if(img->pixel_map == NULL)
    {
        stbi_image_free(img->pixel_map);
        free(img);
        img = NULL;
    }
    return img;
}

void Assets_DrawImage(void* opt)
{
    AssetDraw_T* opts = (AssetDraw_T*)opt;
    Assets_T* a = (Assets_T*)opts->asset;
    Image_T* img = (Image_T*)a->asset;
    if(img)
    {
        glRasterPos2i(opts->options.off_x, opts->options.off_y);
        glDrawPixels(img->width, img->height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, img->pixel_map);
    }
}

bool Assets_RedrawImage(clock_t ms, void* opts)
{
    (void)ms;
    (void)opts;
    return false;
}
