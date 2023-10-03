#include "asset.h"

/* OpenGL includes */
#include "GL/gl.h"
#include "GL/glut.h"

/* Other includes */
#include <string.h>
#include <libgen.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_GIF
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#include "stb_image.h"

/* IMAGE HANDLING */
void draw_image(void* opt)
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

static bool redraw_image(clock_t ms, void* opts)
{
    (void)ms;
    (void)opts;
    return false;
}

/* GIF HANDLING */
static bool gif_redraw(clock_t ms, void* opts)
{
    bool redraw = false;
    AssetDraw_T* op = (AssetDraw_T*)opts;
    Assets_T* asset = (Assets_T*)op->asset;
    GIF_T* gif = (GIF_T*)asset->asset;
    U16* gif_speed = (U16*)op->options.extra;

    if(gif)
    {
        op->status.last_ms += ms;
        U16 delay = 0u;
        if(*gif_speed < GIF_PLAYBACK_SPD_1X)
        {
            delay = gif->delays[op->status.gif_frame % gif->frames] * *gif_speed;
        }
        else
        {
            delay = (U16)((float)gif->delays[op->status.gif_frame % gif->frames] / MAX(0.0f, (*gif_speed / 100.f)));
        }

        if(op->status.last_ms > delay)
        {
            op->status.gif_frame = (op->status.gif_frame + 1u) % gif->frames;
            op->status.last_ms = 0u;
            redraw = true;
        }
    }
    return redraw;
}

static stbi_uc* gif_frame(GIF_T* gif, AssetDraw_Status_T* stat)
{
    return (gif->frame_buff + (((gif->width * gif->channels) * gif->height) * (stat->gif_frame % gif->frames)));
}

static void draw_gif(void* opts)
{
    AssetDraw_T* draw = (AssetDraw_T*)opts;
    Assets_T* a = (Assets_T*)draw->asset;
    GIF_T* gif = (GIF_T*)a->asset;

    if(gif)
    {
        glRasterPos2i(draw->options.off_x, draw->options.off_y);
        glDrawPixels(gif->width, gif->height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, gif_frame(gif, &draw->status));
    }
}

void Assets_AddToScene(Scene_T* scene, void* asset, const char* name, AssetType_e type, AssetRedrawCallback_T redraw, AssetDrawCallback_T draw)
{
    Assets_T* a = malloc(sizeof(Assets_T));
    a->asset = asset;
    a->type = type;

    a->redraw = redraw;
    a->draw = draw;

    char* s = basename(name);
    a->name = malloc(strlen(s) + 1u);
    strcpy(a->name, s);

    HT_Insert(scene->table, s, a);
}

Scene_T* Assets_SetupScene(void)
{
    Scene_T* scene = malloc(sizeof(Scene_T));
    scene->draw_queues[0u] = create_queue();
    scene->draw_queues[1u] = create_queue();
    scene->current_draw_queue = 0u;
    scene->table = malloc(sizeof(HashTable_T));
    return scene;
}

void Assets_Draw(const char* name, Scene_T* scene, AssetDraw_Opts_T opts)
{
    if(scene->table != NULL)
    {
        Assets_T* asset = (Assets_T*)HT_Get(scene->table, name);
        if(asset != NULL)
        {
            AssetDraw_T* d = (AssetDraw_T*)front(scene->draw_queues[scene->current_draw_queue ^ 1u]);
            if((d != NULL) &&
            (memcmp(d->asset, asset, sizeof(Assets_T)) == 0))
            {
                d = (AssetDraw_T*)dequeue(scene->draw_queues[scene->current_draw_queue ^ 1u]);
            }
            else
            {
                d = malloc(sizeof(AssetDraw_T));
                d->asset = asset;
                d->options = opts;
            }

            /* adjust the position if required per redraw */
            d->options.off_x = opts.off_x;
            d->options.off_y = opts.off_y;

            /* actually draw */
            asset->draw(d);
            enqueue(scene->draw_queues[scene->current_draw_queue], d);
        }
        else
        {
            AX_DEBUG("asset not loaded.. (should add some hot loading here)");
        }
    }
}

bool Assets_PromptRedraw(Scene_T* scene, clock_t ms)
{
    bool redraw = false;
    AssetDraw_T* e = NULL;

    U8 pos = 0u;
    while((e = (AssetDraw_T*)peek(scene->draw_queues[scene->current_draw_queue], pos)) != NULL)
    {
        redraw = e->asset->redraw(ms, e) || redraw;
        ++pos;
    }

    return redraw;
}


void Assets_Load(const char* fname, AssetType_e type, Scene_T* scene)
{
    switch(type)
    {
        case ASSET_IMAGE:
        {
            Assets_AddToScene(scene, (void*)Assets_LoadImage(scene, fname), fname, type, redraw_image, draw_image);
            break;
        }

        case ASSET_GIF:
        {
            Assets_AddToScene(scene, (void*)Assets_LoadGIF(scene, fname), fname, type, gif_redraw, draw_gif);
            break;
        }

        default:
            break;
    }
}

void Assets_FlipDrawQueue(Scene_T* scene)
{
    scene->current_draw_queue ^= 1u;
}

void Assets_ClearDrawQueue(Scene_T* scene)
{
    AssetDraw_T* d = NULL;
    while((d = dequeue(scene->draw_queues[scene->current_draw_queue])) != NULL)
    {
        free(d);
    }
}

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

GIF_T* Assets_LoadGIF(Scene_T* scene, const char* fname)
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
    }
    else
    {
        free(gif);
        gif = NULL;
    }
    return gif;
}
