#include "asset.h"

/* OpenGL includes */
#include "GL/gl.h"
#include "GL/glut.h"

/* Other includes */
#include <string.h>
#include <libgen.h>

#include "gif.h"
#include "image.h"

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
