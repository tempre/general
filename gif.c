#include "gif.h"

/* OpenGL includes */
#include "GL/gl.h"
#include "GL/glut.h"

static bool Assets_RedrawGIF(clock_t ms, void* opts)
{
    bool redraw = false;
    AssetDraw_T* op = (AssetDraw_T*)opts;
    Assets_T* asset = (Assets_T*)op->asset;
    GIF_T* gif = (GIF_T*)asset->asset;
    GIF_DrawOpts_T* extra = (GIF_DrawOpts_T*)op->options.extra;
    U16* gif_speed = &extra->speed;

    if(gif)
    {
        op->status.last_ms += ms;
        U16 delay = 0u;
        if(*gif_speed < GIF_PLAYBACK_SPD_1X)
        {
            delay = gif->delays[op->status.frame % gif->frames] * *gif_speed;
        }
        else
        {
            delay = (U16)((float)gif->delays[op->status.frame % gif->frames] / MAX(0.0f, (*gif_speed / 100.f)));
        }

        if(op->status.last_ms > delay)
        {
            op->status.frame = (op->status.frame + 1u) % gif->frames;
            op->status.last_ms = 0u;
            redraw = true;
        }
    }
    return redraw;
}

static stbi_uc* gif_frame(GIF_T* gif, U16 gif_frame)
{
    return (gif->frame_buff + (((gif->width * gif->channels) * gif->height) * (gif_frame % gif->frames)));
}

static void Assets_DrawGIF(void* opts)
{
    AssetDraw_T* draw = (AssetDraw_T*)opts;
    Assets_T* a = (Assets_T*)draw->asset;
    GIF_T* gif = (GIF_T*)a->asset;

    if(gif)
    {
        glRasterPos2i(draw->options.off_x, draw->options.off_y);
        glDrawPixels(gif->width, gif->height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, gif_frame(gif, draw->status.frame));
    }
}

GIF_T* Assets_LoadGIF(const char* fname, Scene_T* scene)
{
    GIF_T* gif = malloc(sizeof(GIF_T));
    FILE* f = fopen(fname, "rb");

    /* file does not exist exit */
    if(f != NULL)
    {
        fseek(f, 0L, SEEK_END);
        gif->file_size = (int)ftell(f);
        gif->data = (stbi_uc*)malloc(gif->file_size);
        fseek(f, 0L, SEEK_SET);
        fread(gif->data, gif->file_size, 1u, f);
        fclose(f);

        stbi_set_flip_vertically_on_load(true);
        gif->frame_buff = stbi_load_gif_from_memory(gif->data, gif->file_size, &gif->delays, &gif->width, &gif->height, &gif->frames, &gif->channels, 0);

        if(gif->frame_buff == NULL)
        {
            stbi_image_free(gif->frame_buff);
            free(gif->data);
            free(gif);
            gif = NULL;
        }
        else
        {
            Assets_AddToScene(scene, gif, fname, ASSET_GIF, Assets_RedrawGIF, Assets_DrawGIF);
        }
    }
    else
    {
        free(gif);
        gif = NULL;
    }
    return gif;
}
