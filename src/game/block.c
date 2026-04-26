#include "game/block.h"
#include "app-context.h"
#include "cglm/mat4.h"
#include "cglm/types.h"
#include "cglm/vec4.h"
#include "render/mesh.h"
#include "render/shader.h"
#include "render/type-vertex.h"

union Shadows {
	uint8_t combined;
	struct {
		bool north : 1;
		bool east : 1;
		bool south : 1;
		bool west : 1;
	} bits;
};
static void Block_GenerateMesh(Block *self, uint8_t character, const vec2 size, uint8_t shadows);
static union Shadows decodeShadows(uint8_t shadows);
uint8_t Block_Shadows(bool north, bool east, bool south, bool west);

Block Block_Create(AppContext *appContext, uint8_t character, vec3 position, vec2 size, uint8_t shadows)
{
	Block result;
	result.appContext = appContext;

	Block_GenerateMesh(&result, character, size, shadows);
	result.position[0] = position[0];
	result.position[1] = position[1];
	result.position[2] = position[2];

	return result;
}
void Block_Destroy(Block *block)
{
	Mesh_Destroy(&block->meshGrass);
}
void Block_Render(const Block self)
{
	Shader_Apply(self.appContext->shaderSolid);

	mat4 matViewProjection = {
		{self.appContext->matrixViewProjection[0][0], self.appContext->matrixViewProjection[0][1], self.appContext->matrixViewProjection[0][2], self.appContext->matrixViewProjection[0][3]},
		{self.appContext->matrixViewProjection[1][0], self.appContext->matrixViewProjection[1][1], self.appContext->matrixViewProjection[1][2], self.appContext->matrixViewProjection[1][3]},
		{self.appContext->matrixViewProjection[2][0], self.appContext->matrixViewProjection[2][1], self.appContext->matrixViewProjection[2][2], self.appContext->matrixViewProjection[2][3]},
		{self.appContext->matrixViewProjection[3][0], self.appContext->matrixViewProjection[3][1], self.appContext->matrixViewProjection[3][2], self.appContext->matrixViewProjection[3][3]},
	};
	mat4 matModel = GLM_MAT4_IDENTITY_INIT;
	vec3 position = {self.position[0], self.position[1], self.position[2]};
	glm_translate(matModel, position);
	mat4 matMVP = GLM_MAT4_IDENTITY_INIT;
	glm_mat4_mul(matViewProjection, matModel, matMVP);
	Shader_SetMVP(self.appContext->shaderSolid, matMVP);
	Mesh_Render(self.meshGrass);
}

#define BLOCK_GRASS_COLOR_INIT {0.5f, 0.7f, 0.3f, 1.0f}
#define BLOCK_GRASS_SHADOW_COLOR_INIT {0.3f, 0.45f, 0.2f, 1.0f}
#define BLOCK_GRASS_SYMBOL_COLOR_INIT {0.2f, 0.35f, 0.1f, 1.0f}
#define BLOCK_GROUND_COLOR_INIT {0.6f, 0.4f, 0.3f, 1.0f}
#define BLOCK_GROUND_SHADOW_COLOR_INIT {0.3f, 0.2f, 0.1f, 1.0}
#define BLOCK_GROUND_SYMBOL_COLOR_INIT {0.4f, 0.25f, 0.1f, 1.0}
#define BLOCK_GRASS_COLOR ((vec4)BLOCK_GRASS_COLOR_INIT)
#define BLOCK_GRASS_SHADOW_COLOR ((vec4)BLOCK_GRASS_SHADOW_COLOR_INIT)
#define BLOCK_GRASS_SYMBOL_COLOR ((vec4)BLOCK_GRASS_SYMBOL_COLOR_INIT)
#define BLOCK_GROUND_COLOR ((vec4)BLOCK_GROUND_COLOR_INIT)
#define BLOCK_GROUND_SHADOW_COLOR ((vec4)BLOCK_GROUND_SHADOW_COLOR_INIT)
#define BLOCK_GROUND_SYMBOL_COLOR ((vec4)BLOCK_GROUND_SYMBOL_COLOR_INIT)
#define VEC4_INIT_FROM(VEC) {(VEC)[0], (VEC)[1], (VEC)[2], (VEC)[3]}

static bool Block_IsGrass(uint8_t character);

#define BLOCK_MAX_SYMBOL_GEOMETRY_VERTICES 22
#define BLOCK_MESH_BLOCK_VERTICES ((int)(4 * 6))
#define BLOCK_MESH_MAX_VERTICES ((int)(BLOCK_MESH_BLOCK_VERTICES + BLOCK_MAX_SYMBOL_GEOMETRY_VERTICES * 2))
#define BLOCK_CORNER_CUT_WIDTH ((float)(1.0f / 0x10))
#define BLOCK_HEIGHT 1.0f
#define BLOCK_INDICES_STATIC_COUNT ((int)(5 * 6 + 12 * 6 + 8 * 3))

static void Block_GenerateSurface1(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol);
static void Block_GenerateSurface2(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol);
static void Block_GenerateSurface3(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol);
static void Block_GenerateSurface4(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol);
static void Block_GenerateSurface5(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol);
static void Block_GenerateSurface6(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol);
static void Block_GenerateSurface7(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol);
static void Block_GenerateSurface8(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol);
static void Block_GenerateSurfaceX(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol);

static void Block_GenerateMesh(Block *self, uint8_t character, const vec2 size, uint8_t shadowsRaw)
{
	union Shadows shadows = decodeShadows(shadowsRaw);
	(void)shadows;
	const bool isGrass = Block_IsGrass(character);

	vec4 colorBase = BLOCK_GROUND_COLOR_INIT;
	vec4 colorShadows = BLOCK_GROUND_SHADOW_COLOR_INIT;
	vec4 colorSymbol = BLOCK_GROUND_SYMBOL_COLOR_INIT;
	if (isGrass) {
		glm_vec4_copy(BLOCK_GRASS_COLOR, colorBase);
		glm_vec4_copy(BLOCK_GRASS_SHADOW_COLOR, colorShadows);
		glm_vec4_copy(BLOCK_GRASS_SYMBOL_COLOR, colorSymbol);
	}

	vec4 colorWest = VEC4_INIT_FROM((shadows.bits.west ? colorShadows : colorBase));
	vec4 colorSouth = VEC4_INIT_FROM((shadows.bits.south ? colorShadows : colorBase));
	vec4 colorEast = VEC4_INIT_FROM((shadows.bits.east ? colorShadows : colorBase));
	vec4 colorNorth = VEC4_INIT_FROM((shadows.bits.north ? colorShadows : colorBase));
	VertexSolid vertices[BLOCK_MESH_MAX_VERTICES] = {
		// bottom side
		/* 0*/ (VertexSolid){.position = {BLOCK_CORNER_CUT_WIDTH, -BLOCK_HEIGHT, BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorShadows)},
		/* 1*/ (VertexSolid){.position = {size[0] - BLOCK_CORNER_CUT_WIDTH, -BLOCK_HEIGHT, BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorShadows)},
		/* 2*/ (VertexSolid){.position = {size[0] - BLOCK_CORNER_CUT_WIDTH, -BLOCK_HEIGHT, size[1] - BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorShadows)},
		/* 3*/ (VertexSolid){.position = {BLOCK_CORNER_CUT_WIDTH, -BLOCK_HEIGHT, size[1] - BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorShadows)},

		// west side
		/* 4*/ (VertexSolid){.position = {0.0f, -BLOCK_CORNER_CUT_WIDTH, size[1] - BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorWest)},
		/* 5*/ (VertexSolid){.position = {0.0f, -BLOCK_CORNER_CUT_WIDTH, BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorWest)},
		/* 6*/ (VertexSolid){.position = {0.0f, -BLOCK_HEIGHT + BLOCK_CORNER_CUT_WIDTH, BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorWest)},
		/* 7*/ (VertexSolid){.position = {0.0f, -BLOCK_HEIGHT + BLOCK_CORNER_CUT_WIDTH, size[1] - BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorWest)},

		// south side
		/* 8*/ (VertexSolid){.position = {BLOCK_CORNER_CUT_WIDTH, -BLOCK_CORNER_CUT_WIDTH, 0.0f}, .color = VEC4_INIT_FROM(colorSouth)},
		/* 9*/ (VertexSolid){.position = {size[0] - BLOCK_CORNER_CUT_WIDTH, -BLOCK_CORNER_CUT_WIDTH, 0.0f}, .color = VEC4_INIT_FROM(colorSouth)},
		/*10*/ (VertexSolid){.position = {size[0] - BLOCK_CORNER_CUT_WIDTH, -BLOCK_HEIGHT + BLOCK_CORNER_CUT_WIDTH, 0.0f}, .color = VEC4_INIT_FROM(colorSouth)},
		/*11*/ (VertexSolid){.position = {BLOCK_CORNER_CUT_WIDTH, -BLOCK_HEIGHT + BLOCK_CORNER_CUT_WIDTH, 0.0f}, .color = VEC4_INIT_FROM(colorSouth)},

		// east side
		/*12*/ (VertexSolid){.position = {size[0], -BLOCK_CORNER_CUT_WIDTH, BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorEast)},
		/*13*/ (VertexSolid){.position = {size[0], -BLOCK_CORNER_CUT_WIDTH, size[1] - BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorEast)},
		/*14*/ (VertexSolid){.position = {size[0], -BLOCK_HEIGHT + BLOCK_CORNER_CUT_WIDTH, size[1] - BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorEast)},
		/*15*/ (VertexSolid){.position = {size[0], -BLOCK_HEIGHT + BLOCK_CORNER_CUT_WIDTH, BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorEast)},

		// north side
		/*16*/ (VertexSolid){.position = {size[0] - BLOCK_CORNER_CUT_WIDTH, -BLOCK_CORNER_CUT_WIDTH, size[1]}, .color = VEC4_INIT_FROM(colorNorth)},
		/*17*/ (VertexSolid){.position = {BLOCK_CORNER_CUT_WIDTH, -BLOCK_CORNER_CUT_WIDTH, size[1]}, .color = VEC4_INIT_FROM(colorNorth)},
		/*18*/ (VertexSolid){.position = {BLOCK_CORNER_CUT_WIDTH, -BLOCK_HEIGHT + BLOCK_CORNER_CUT_WIDTH, size[1]}, .color = VEC4_INIT_FROM(colorNorth)},
		/*19*/ (VertexSolid){.position = {size[0] - BLOCK_CORNER_CUT_WIDTH, -BLOCK_HEIGHT + BLOCK_CORNER_CUT_WIDTH, size[1]}, .color = VEC4_INIT_FROM(colorNorth)},

		// top side
		/*20*/ (VertexSolid){.position = {BLOCK_CORNER_CUT_WIDTH, 0.0f, BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorBase)},
		/*21*/ (VertexSolid){.position = {BLOCK_CORNER_CUT_WIDTH, 0.0f, size[1] - BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorBase)},
		/*22*/ (VertexSolid){.position = {size[0] - BLOCK_CORNER_CUT_WIDTH, 0.0f, size[1] - BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorBase)},
		/*23*/ (VertexSolid){.position = {size[0] - BLOCK_CORNER_CUT_WIDTH, 0.0f, BLOCK_CORNER_CUT_WIDTH}, .color = VEC4_INIT_FROM(colorBase)},

	};

	// clang-format off
	uint8_t indices[] = {
		// static part
		/// sides
		0, 1, 2, 0, 2, 3, // bottom
		4, 5, 6, 4, 6, 7, // west
		8, 9, 10, 8, 10, 11, // south
		12, 13, 14, 12, 14, 15, // east
		16, 17, 18, 16, 18, 19, // north
		/// edges
		5, 8, 11, 5, 11, 6,
		17, 4, 7, 17, 7, 18,
		13, 16, 19, 13, 19, 14,
		9, 12, 15, 9, 15, 10,
		20, 23, 9, 20, 9, 8,
		21, 20, 5, 21, 5, 4,
		22, 21, 17, 22, 17, 16,
		23, 22, 13, 23, 13, 12,
		11, 10, 1, 11, 1, 0,
		7, 6, 0, 7, 0, 3,
		19, 18, 3, 19, 3, 2,
		15, 14, 2, 15, 2, 1,
		/// corners
		20, 8, 5,
		21, 4, 17,
		22, 16, 13,
		23, 12, 9,
		6, 11, 0,
		18, 7, 3,
		14, 19, 2,
		10, 15, 1,
		/// dynamic part
		//20, 21, 22, 20, 22, 23, // top
		11 + BLOCK_MESH_BLOCK_VERTICES +  0, 11 + BLOCK_MESH_BLOCK_VERTICES +  1, 11 + BLOCK_MESH_BLOCK_VERTICES +  2,
		11 + BLOCK_MESH_BLOCK_VERTICES + 10, 11 + BLOCK_MESH_BLOCK_VERTICES +  0, 11 + BLOCK_MESH_BLOCK_VERTICES +  2,
		11 + BLOCK_MESH_BLOCK_VERTICES + 10, 11 + BLOCK_MESH_BLOCK_VERTICES +  2, 11 + BLOCK_MESH_BLOCK_VERTICES +  3,
		11 + BLOCK_MESH_BLOCK_VERTICES +  4, 11 + BLOCK_MESH_BLOCK_VERTICES + 10, 11 + BLOCK_MESH_BLOCK_VERTICES +  3,
		11 + BLOCK_MESH_BLOCK_VERTICES +  9, 11 + BLOCK_MESH_BLOCK_VERTICES + 10, 11 + BLOCK_MESH_BLOCK_VERTICES +  4,
		11 + BLOCK_MESH_BLOCK_VERTICES +  4, 11 + BLOCK_MESH_BLOCK_VERTICES +  5, 11 + BLOCK_MESH_BLOCK_VERTICES +  6,
		11 + BLOCK_MESH_BLOCK_VERTICES +  4, 11 + BLOCK_MESH_BLOCK_VERTICES +  6, 11 + BLOCK_MESH_BLOCK_VERTICES +  9,
		11 + BLOCK_MESH_BLOCK_VERTICES +  9, 11 + BLOCK_MESH_BLOCK_VERTICES +  6, 11 + BLOCK_MESH_BLOCK_VERTICES +  7,
		11 + BLOCK_MESH_BLOCK_VERTICES +  7, 11 + BLOCK_MESH_BLOCK_VERTICES +  8, 11 + BLOCK_MESH_BLOCK_VERTICES +  9,

		BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES + 10, BLOCK_MESH_BLOCK_VERTICES +  9,
		BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES +  9, BLOCK_MESH_BLOCK_VERTICES +  8,
		BLOCK_MESH_BLOCK_VERTICES +  0,                             21, BLOCK_MESH_BLOCK_VERTICES +  1,
		BLOCK_MESH_BLOCK_VERTICES +  1,                             21, BLOCK_MESH_BLOCK_VERTICES +  2,
		                            21, BLOCK_MESH_BLOCK_VERTICES +  3, BLOCK_MESH_BLOCK_VERTICES +  2,
		BLOCK_MESH_BLOCK_VERTICES +  3,                             21,                             22,
		BLOCK_MESH_BLOCK_VERTICES +  3, BLOCK_MESH_BLOCK_VERTICES +  5, BLOCK_MESH_BLOCK_VERTICES +  4,
		BLOCK_MESH_BLOCK_VERTICES +  3,                             22, BLOCK_MESH_BLOCK_VERTICES +  5,
		BLOCK_MESH_BLOCK_VERTICES +  5,                             22,                             23,
		BLOCK_MESH_BLOCK_VERTICES +  5,                             23, BLOCK_MESH_BLOCK_VERTICES +  6,
		BLOCK_MESH_BLOCK_VERTICES +  6,                             23, BLOCK_MESH_BLOCK_VERTICES +  7,
		BLOCK_MESH_BLOCK_VERTICES +  7,                             23,                             20,
		BLOCK_MESH_BLOCK_VERTICES +  8, BLOCK_MESH_BLOCK_VERTICES +  7,                             20,
		BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES +  8,                             20,
		BLOCK_MESH_BLOCK_VERTICES +  0,                             20,                             21,
	};
	// clang-format on

	Block_GenerateSurface1(size, &vertices[BLOCK_MESH_BLOCK_VERTICES], indices, colorBase, colorSymbol);

	self->meshGrass = Mesh_Create(vertexSolidDescription, vertexSolidDescriptionLength, vertices, sizeof(vertices), indices, sizeof(indices), MeshIndexTypeUint8);
}
static union Shadows decodeShadows(uint8_t shadows)
{
	union Shadows decoded = {.combined = shadows};
	return decoded;
}
uint8_t Block_Shadows(bool north, bool east, bool south, bool west)
{
	union Shadows encoded = {.bits.north = north, .bits.east = east, .bits.south = south, .bits.west = west};
	return encoded.combined;
}
static bool Block_IsGrass(uint8_t character)
{
	return (character == BLOCK_GRASS || character == BLOCK_X);
}
// '1': {{0.3125f, 0.4375f}, {0.3125f, 0.375f}, {0.5f, 0.1875f}, {0.625f, 0.1875f}, {0.625f, 0.6875f}, {0.75f, 0.6875f}, {0.75f, 0.8125f}, {0.3125f, 0.8125f}, {0.3125f, 0.6875f}, {0.4375f, 0.6875f}, {0.4375f, 0.4375f}};
static void Block_GenerateSurface1(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol)
{
	const vec2 surface[] = {{0.3125f, 0.4375f}, {0.3125f, 0.375f}, {0.5f, 0.1875f}, {0.625f, 0.1875f}, {0.625f, 0.6875f}, {0.75f, 0.6875f}, {0.75f, 0.8125f}, {0.3125f, 0.8125f}, {0.3125f, 0.6875f}, {0.4375f, 0.6875f}, {0.4375f, 0.4375f}};
	const size_t surfaceLength = sizeof(surface) / sizeof(surface[0]);
	const vec2 offset = {size[0] * 0.5f - 0.5f, size[1] * 0.5f + 0.5f};
	for (size_t i = 0; i < surfaceLength; i++) {
		vertices[i] = (VertexSolid){.position = {offset[0] + surface[i][0], 0.0f, offset[1] - surface[i][1]}, .color = VEC4_INIT_FROM(colorBase)};
		vertices[surfaceLength + i] = (VertexSolid){.position = {offset[0] + surface[i][0], 0.0f, offset[1] - surface[i][1]}, .color = VEC4_INIT_FROM(colorSymbol)};
	}
}
// '2': {{0.1875f, 0.375f}, {0.1875f, 0.25f}, {0.25f, 0.1875f}, {0.75f, 0.1875f}, {0.8125f, 0.25f}, {0.8125f, 0.4375f}, {0.8125f, 0.5f}, {0.4375f, 0.6875f}, {0.8125f, 0.6875f}, {0.8125f, 0.8125f}, {0.1875f, 0.8125f}, {0.1875f, 0.65625f}, {0.625f, 0.4375f}, {0.625f, 0.3125f}, {0.375f, 0.3125f}, {0.375f, 0.375f}};
static void Block_GenerateSurface2(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol)
{
}
// '3': {{0.1875f, 0.1875f}, {0.75f, 0.1875f}, {0.8125f, 0.25f}, {0.8125f, 0.4375f}, {0.75f, 0.5f}, {0.8125f, 0.5625f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.1875f, 0.8125f}, {0.1875f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}, {0.375f, 0.5625f}, {0.375f, 0.4375f}, {0.625f, 0.4375f}, {0.625f, 0.3125f}, {0.1875f, 0.3125f}};
static void Block_GenerateSurface3(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol)
{
}
// '4': {{0.1875f, 0.4375f}, {0.3125f, 0.1875f}, {0.5f, 0.1875f}, {0.375f, 0.4375f}, {0.5625f, 0.4375f}, {0.5625f, 0.1875f}, {0.75f, 0.1875f}, {0.75f, 0.4375f}, {0.8125f, 0.4375f}, {0.8125f, 0.5625f}, {0.75f, 0.5625f}, {0.75f, 0.8125f}, {0.5625f, 0.8125f}, {0.5625f, 0.5625f}, {0.1875f, 0.5625f}};
static void Block_GenerateSurface4(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol)
{
}
// '5': {{0.1875f, 0.1875f}, {0.8125f, 0.1875f}, {0.8125f, 0.3125f}, {0.375f, 0.3125f}, {0.375f, 0.4375f}, {0.75f, 0.4375f}, {0.8125f, 0.5f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.1875f, 0.8125f}, {0.1875f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}, {0.1875f, 0.5625f}};
static void Block_GenerateSurface5(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol)
{
}
// '6': {{0.25f, 0.1875f}, {0.8125f, 0.1875f}, {0.8125f, 0.3125f}, {0.375f, 0.3125f}, {0.375f, 0.4375f}, {0.75f, 0.4375f}, {0.8125f, 0.5f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.25f, 0.8125f}, {0.1875f, 0.75f}, {0.1875f, 0.25f}, {0.375f, 0.5625f}, {0.375f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}};
static void Block_GenerateSurface6(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol)
{
}
// '7': {{0.1875f, 0.1875f}, {0.8125f, 0.1875f}, {0.8125f, 0.3125f}, {0.5625f, 0.8125f}, {0.375f, 0.8125f}, {0.625f, 0.3125f}, {0.1875f, 0.3125f}};
static void Block_GenerateSurface7(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol)
{
}
// '8': {{0.1875f, 0.25f}, {0.25f, 0.1875f}, {0.75f, 0.1875f}, {0.8125f, 0.25f}, {0.8125f, 0.4375f}, {0.75f, 0.5f}, {0.8125f, 0.5625f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.25f, 0.8125f}, {0.1875f, 0.75f}, {0.1875f, 0.5625f}, {0.25f, 0.5f}, {0.1875f, 0.4375f}, {0.375f, 0.3125f}, {0.375f, 0.4375f}, {0.625f, 0.4375f}, {0.625f, 0.3125f}, {0.375f, 0.5625f}, {0.375f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}};
static void Block_GenerateSurface8(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol)
{
}
// 'x': {{0.1875f, 0.3125f}, {0.3125f, 0.1875f}, {0.5f, 0.375f}, {0.6875f, 0.1875f}, {0.8125f, 0.3125f}, {0.625f, 0.5f}, {0.8125f, 0.6875f}, {0.6875f, 0.8125f}, {0.5f, 0.625f}, {0.3125f, 0.8125f}, {0.1875f, 0.6875f}, {0.375f, 0.5f}};
static void Block_GenerateSurfaceX(const vec2 size, VertexSolid *vertices, uint8_t *indices, vec4 colorBase, vec4 colorSymbol)
{
}
