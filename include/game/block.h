#ifndef __GAME_BLOCK_H__
#define __GAME_BLOCK_H__

#include "app-context-pre.h"
#include "cglm/cglm.h" // IWYU pragma: export
#include "render/mesh.h"
#include <stdbool.h>

#define BLOCK_GRASS ((int)' ')
#define BLOCK_X ((int)'x')
#define BLOCK_DIRT ((int)'0')
#define BLOCK_1 ((int)'1')
#define BLOCK_2 ((int)'2')
#define BLOCK_3 ((int)'3')
#define BLOCK_4 ((int)'4')
#define BLOCK_5 ((int)'5')
#define BLOCK_6 ((int)'6')
#define BLOCK_7 ((int)'7')
#define BLOCK_8 ((int)'8')

typedef struct Block {
	AppContext *appContext;

	Mesh meshGround;
	Mesh meshGrass;
	vec3 position;
} Block;

uint8_t Block_Shadows(bool north, bool east, bool south, bool west);

Block Block_Create(AppContext *appContext, uint8_t character, vec3 position, vec2 size, uint8_t shadows);
void Block_Destroy(Block *block);
void Block_Render(const Block self);

#endif
