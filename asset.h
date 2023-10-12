#ifndef ASSET_H_
#define ASSET_H_

#include "common.h"
#include "q.h"
#include "hash.h"

#define ASSET_GIPHY_GIF        "giphy.gif"
#define ASSET_CAT_GIF          "cat.gif"
#define ASSET_KIKI_PNG         "kiki.png"
#define ASSET_PENG_GIF         "peng.gif"

#define DRAW_QUEUE_SIZE 2u

typedef enum {
    ASSET_IMAGE,
    ASSET_GIF,
    ASSET_CHARMAP,
} AssetType_e;

typedef bool (*AssetRedrawCallback_T)(clock_t ms, void* opts);
typedef void (*AssetDrawCallback_T)(void* opts);

typedef struct Assets_T {
    char* name;
    void* asset;

    AssetRedrawCallback_T redraw;
    AssetDrawCallback_T draw;
    AssetType_e type;

    struct Assets_T* next;
    struct Assets_T* prev;
} Assets_T;

typedef struct {
    /* general */
    U16 off_x;
    U16 off_y;

    void* extra;
} AssetDraw_Opts_T;

typedef struct {
    clock_t last_ms;
} AssetDraw_Status_T;

typedef struct {
    Assets_T* asset;

    /* status of draw */
    AssetDraw_Status_T status;
    /* options */
    AssetDraw_Opts_T options;
} AssetDraw_T;

typedef struct {
  HashTable_T* table;

  U8 current_draw_queue;
  Queue_T* draw_queues[DRAW_QUEUE_SIZE];
} Scene_T;

void Assets_AddToScene(Scene_T* scene, void* asset, const char* name, AssetType_e type, AssetRedrawCallback_T redraw, AssetDrawCallback_T draw);
Scene_T* Assets_SetupScene(void);
void Assets_Draw(const char* name, Scene_T* scene, AssetDraw_Opts_T opts);
bool Assets_PromptRedraw(Scene_T* scene, clock_t ms);
void Assets_Load(const char* fname, AssetType_e type, Scene_T* scene);
void Assets_FlipDrawQueue(Scene_T* scene);
void Assets_ClearDrawQueue(Scene_T* scene);

#endif // ASSET_H_
