#ifndef __GAME_GRID_H__
#define __GAME_GRID_H__

typedef struct Grid Grid;
#include "app-context-pre.h"
#include "game/block.h"
#include <stdint.h>

// use fixed sized field
#define GRID_WIDTH 16
#define GRID_HEIGHT 16
struct Grid {
	AppContext *appContext;

	Block field[GRID_WIDTH * GRID_HEIGHT];
};

Grid Grid_Create(AppContext *appContext, uint32_t width, uint32_t height);
void Grid_Destroy(Grid *grid);
void Grid_Render(const Grid self);
void Grid_Click(Grid *self, uint32_t x, uint32_t y);

#endif