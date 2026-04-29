#ifndef __GAME_GRID_H__
#define __GAME_GRID_H__

typedef struct Grid Grid;
#include "app-context-pre.h"
#include "game/block.h"
#include <stdint.h>

// use fixed sized field
#define GRID_WIDTH 16
#define GRID_HEIGHT 16
#define WIDTH_BYTES(WIDTH) ((int)((WIDTH + 7) / 8))
#define GRID_WIDTH_BYTES WIDTH_BYTES(GRID_WIDTH)
#define MINES_COUNT 40
struct Grid {
	AppContext *appContext;

	uint8_t mines[WIDTH_BYTES(GRID_WIDTH) * GRID_HEIGHT];
	uint8_t flags[WIDTH_BYTES(GRID_WIDTH) * GRID_HEIGHT];
	uint8_t digMask[WIDTH_BYTES(GRID_WIDTH) * GRID_HEIGHT];
	Block field[GRID_WIDTH * GRID_HEIGHT];

	double time;
};

Grid Grid_Create(AppContext *appContext);
void Grid_Destroy(Grid *grid);
void Grid_Update(Grid *self, double deltaTime);
void Grid_Render(const Grid *self);
void Grid_Click(Grid *self, uint32_t x, uint32_t y);

#endif
