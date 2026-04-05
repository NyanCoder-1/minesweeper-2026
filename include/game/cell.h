#ifndef __GAME_CELL_H__
#define __GAME_CELL_H__

#include "app-context-pre.h"
#include "cglm/cglm.h" // IWYU pragma: export
#include "render/mesh.h"

typedef struct Cell {
	AppContext *appContext;

	Mesh mesh;
	vec3 position;
} Cell;

Cell Cell_Create(AppContext *appContext, vec3 position, vec2 size);

#endif
