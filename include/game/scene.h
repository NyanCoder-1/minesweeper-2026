#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

typedef struct Scene Scene;
#include "app-context-pre.h"
#include "game/grid.h"
struct Scene {
	AppContext *appContext;

	Grid field;
	Block block;
};

Scene Scene_Create(AppContext *appContext);
void Scene_Destroy(Scene *scene);
void Scene_Update(Scene *self);
void Scene_Render(const Scene self);

#endif