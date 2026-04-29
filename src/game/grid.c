#include "game/grid.h"
#include <math.h>
#include <string.h>
#include <time.h>

#define GRID_ANIM_STATE_DURATION 2.0f

static bool Grid_GetBit(const uint8_t *bytes, uint32_t x, uint32_t y);
static void Grid_SetBit(uint8_t *bytes, uint32_t x, uint32_t y, bool bit);
static void Grid_GenerateField(uint8_t *field);
static uint8_t Grid_NeighboursCountToCharacter(int count);

Grid Grid_Create(AppContext *appContext)
{
	Grid grid;
	memset(&grid, 0, sizeof(Grid));

	memset(grid.mines, 0, sizeof(grid.mines));
	Grid_GenerateField(grid.mines);

	const int8_t dx[] = {-1, 0, 1, 1, 1, 0, -1, -1};
	const int8_t dy[] = {-1, -1, -1, 0, 1, 1, 1, 0};
	const uint8_t dLength = sizeof(dx) / sizeof(dx[0]);

	for (int y = 0; y < GRID_HEIGHT; y++) {
		for (int x = 0; x < GRID_WIDTH; x++) {
			if (Grid_GetBit(grid.mines, x, y)) {
				grid.field[y * GRID_WIDTH + x] = Block_Create(appContext, BLOCK_GRASS, (vec3){x, 0.0f, (float)(GRID_HEIGHT - y - 1)}, (vec2){1.0f, 1.0f}, Block_Shadows(true, x < GRID_WIDTH - 1, true, x > 0));
				continue;
			}
			int neighbours = 0;
			for (int i = 0; i < dLength; i++) {
				const int nx = x + dx[i];
				const int ny = y + dy[i];
				if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT) {
					neighbours += Grid_GetBit(grid.mines, nx, ny) ? 1 : 0;
				}
			}
			grid.field[y * GRID_WIDTH + x] = Block_Create(appContext, Grid_NeighboursCountToCharacter(neighbours), (vec3){x, 0.0f, (float)(GRID_HEIGHT - y - 1)}, (vec2){1.0f, 1.0f}, Block_Shadows(true, x < GRID_WIDTH - 1, true, x > 0));
		}
	}
	return grid;
}
void Grid_Destroy(Grid *grid)
{
	if (!grid) return;
	for (int y = 0; y < GRID_HEIGHT; y++) {
		for (int x = 0; x < GRID_WIDTH; x++) {
			const int i = y * GRID_WIDTH + x;
			Block_Destroy(&grid->field[i]);
		}
	}
	memset(grid, 0, sizeof(Grid));
}
void Grid_Update(Grid *self, double deltaTime)
{
	//self->time = fmod(self->time + deltaTime, GRID_ANIM_STATE_DURATION * 2);
}
void Grid_Render(const Grid *self)
{
	for (int y = 0; y < GRID_HEIGHT; y++) {
		for (int x = 0; x < GRID_WIDTH; x++) {
			const int i = y * GRID_WIDTH + x;
			Block_Render(self->field[i], self->time > GRID_ANIM_STATE_DURATION);
		}
	}
}
void Grid_Click(Grid *self, uint32_t x, uint32_t y)
{
}

static uint8_t Grid_NeighboursCountToCharacter(int count) {
	switch (count) {
		case 0:
			return BLOCK_DIRT;
		case 1:
			return BLOCK_1;
		case 2:
			return BLOCK_2;
		case 3:
			return BLOCK_3;
		case 4:
			return BLOCK_4;
		case 5:
			return BLOCK_5;
		case 6:
			return BLOCK_6;
		case 7:
			return BLOCK_7;
		case 8:
			return BLOCK_8;
	}
}
static bool Grid_GetBit(const uint8_t *bytes, uint32_t x, uint32_t y)
{
	const uint8_t bitmasks[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
	const uint8_t remainder = x % 8;
	const uint32_t quotient = x / 8;
	return bytes[y * GRID_WIDTH_BYTES + quotient] & bitmasks[remainder];
}
static void Grid_SetBit(uint8_t *bytes, uint32_t x, uint32_t y, bool bit)
{
	const uint8_t bitmasks[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
	const uint8_t remainder = x % 8;
	const uint32_t quotient = x / 8;
	const uint8_t byte = bytes[y * GRID_WIDTH_BYTES + quotient];
	bytes[y * GRID_WIDTH_BYTES + quotient] = (byte & ~bitmasks[remainder]) | (bit ? bitmasks[remainder] : 0); 
}
static void Grid_ClearField(uint8_t *field);
static void Grid_ScatterMines(uint8_t *field);
static bool Grid_IsWay(const uint8_t *field);
static bool Grid_IsEmptyCell(const uint8_t *field);
static void Grid_GenerateField(uint8_t *field)
{
	srand(time(0));
	bool good = false;
	while (!good) {
		// clear
		Grid_ClearField(field);
		// scatter mines
		Grid_ScatterMines(field);

		// check if there is a way
		bool isWay = Grid_IsWay(field);

		// check if there is an empty cell in the bottom
		bool isEmptyCell = Grid_IsEmptyCell(field);

		good = isWay && isEmptyCell || true;
	}
}
static void Grid_ClearField(uint8_t *field)
{
	for (int y = 0; y < GRID_HEIGHT; y++) {
		for (int x = 0; x < GRID_WIDTH_BYTES; x++) {
			const int i = y * GRID_WIDTH_BYTES + x;
			field[i] = 0;
		}
	}
}
static void Grid_ScatterMines(uint8_t *field)
{
	for (int i = 0; i < MINES_COUNT; i++) {
		uint32_t emptyCells[GRID_WIDTH * GRID_HEIGHT][2];
		uint32_t emptyCellsCount = 0;
		for (int y = 0; y < GRID_HEIGHT; y++) {
			for (int x = 0; x < GRID_WIDTH; x++) {
				if (!Grid_GetBit(field, x, y)) {
					emptyCells[emptyCellsCount][0] = x;
					emptyCells[emptyCellsCount][1] = y;
					emptyCellsCount++;
				}
			}
		}
		uint32_t j = rand() % emptyCellsCount;
		uint32_t x = emptyCells[j][0];
		uint32_t y = emptyCells[j][1];
		Grid_SetBit(field, x, y, true);
	}
}
static bool Grid_IsWay(const uint8_t *field)
{
	return true;
	bool result = false;
	uint8_t paintMap[GRID_WIDTH_BYTES * GRID_HEIGHT];
	Grid_ClearField(paintMap);
	uint8_t neighbours[GRID_WIDTH_BYTES * GRID_HEIGHT];
	Grid_ClearField(neighbours);
	bool painted;
	do {
		painted = false;

	} while (painted);
	return result;
}
static bool Grid_IsEmptyCell(const uint8_t *field)
{
	return true;
}
