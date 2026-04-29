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
#define BLOCK_GRASS_FORCED ((int)('G'))

Block Block_Create(AppContext *appContext, uint8_t character, vec3 position, vec2 size, uint8_t shadows)
{
	Block result;
	result.appContext = appContext;

	switch (character) {
		case BLOCK_1:
		case BLOCK_2:
		case BLOCK_3:
		case BLOCK_4:
		case BLOCK_5:
		case BLOCK_6:
		case BLOCK_7:
		case BLOCK_8:
		case BLOCK_DIRT:
			Block_GenerateMesh(&result, BLOCK_GRASS, size, shadows);
			Block_GenerateMesh(&result, character, size, shadows);
			break;
		case BLOCK_X:
			Block_GenerateMesh(&result, BLOCK_DIRT, size, shadows);
			Block_GenerateMesh(&result, character, size, shadows);
			break;
		case BLOCK_GRASS:
			Block_GenerateMesh(&result, BLOCK_GRASS_FORCED, size, shadows);
			Block_GenerateMesh(&result, character, size, shadows);
			break;
		default:
			Block_GenerateMesh(&result, BLOCK_GRASS, size, shadows);
			Block_GenerateMesh(&result, BLOCK_DIRT, size, shadows);
			break;
	}
	result.position[0] = position[0];
	result.position[1] = position[1];
	result.position[2] = position[2];

	return result;
}
void Block_Destroy(Block *block)
{
	Mesh_Destroy(&block->meshGrass);
}
void Block_Render(const Block self, bool green)
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
	Mesh_Render(green ? self.meshGrass : self.meshGround);
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
#define BLOCK_INDICES_MAX_DYNAMIC_COUNT ((int)(256 * 3))
//#define BLOCK_INDICES_MAX_DYNAMIC_COUNT ((int)(46 * 3))
#define BLOCK_MESH_MAX_INDICES ((int)(BLOCK_INDICES_STATIC_COUNT + BLOCK_INDICES_MAX_DYNAMIC_COUNT))

static void Block_GenerateSurface1(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength);
static void Block_GenerateSurface2(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength);
static void Block_GenerateSurface3(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength);
static void Block_GenerateSurface4(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength);
static void Block_GenerateSurface5(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength);
static void Block_GenerateSurface6(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength);
static void Block_GenerateSurface7(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength);
static void Block_GenerateSurface8(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength);
static void Block_GenerateSurfaceX(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength);

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
	uint8_t indices[BLOCK_MESH_MAX_INDICES] = {
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
		20, 21, 22, 20, 22, 23, // top
	};
	// clang-format on

	size_t verticesLength = BLOCK_MESH_BLOCK_VERTICES;
	size_t indicesLength = BLOCK_INDICES_STATIC_COUNT + 6;
	switch (character) {
		case BLOCK_1:
			Block_GenerateSurface1(size, &vertices[BLOCK_MESH_BLOCK_VERTICES], &indices[BLOCK_INDICES_STATIC_COUNT], colorBase, colorSymbol, &verticesLength, &indicesLength);
			break;
		case BLOCK_2:
			Block_GenerateSurface2(size, &vertices[BLOCK_MESH_BLOCK_VERTICES], &indices[BLOCK_INDICES_STATIC_COUNT], colorBase, colorSymbol, &verticesLength, &indicesLength);
			break;
		case BLOCK_3:
			Block_GenerateSurface3(size, &vertices[BLOCK_MESH_BLOCK_VERTICES], &indices[BLOCK_INDICES_STATIC_COUNT], colorBase, colorSymbol, &verticesLength, &indicesLength);
			break;
		case BLOCK_4:
			Block_GenerateSurface4(size, &vertices[BLOCK_MESH_BLOCK_VERTICES], &indices[BLOCK_INDICES_STATIC_COUNT], colorBase, colorSymbol, &verticesLength, &indicesLength);
			break;
		case BLOCK_5:
			Block_GenerateSurface5(size, &vertices[BLOCK_MESH_BLOCK_VERTICES], &indices[BLOCK_INDICES_STATIC_COUNT], colorBase, colorSymbol, &verticesLength, &indicesLength);
			break;
		case BLOCK_6:
			Block_GenerateSurface6(size, &vertices[BLOCK_MESH_BLOCK_VERTICES], &indices[BLOCK_INDICES_STATIC_COUNT], colorBase, colorSymbol, &verticesLength, &indicesLength);
			break;
		case BLOCK_7:
			Block_GenerateSurface7(size, &vertices[BLOCK_MESH_BLOCK_VERTICES], &indices[BLOCK_INDICES_STATIC_COUNT], colorBase, colorSymbol, &verticesLength, &indicesLength);
			break;
		case BLOCK_8:
			Block_GenerateSurface8(size, &vertices[BLOCK_MESH_BLOCK_VERTICES], &indices[BLOCK_INDICES_STATIC_COUNT], colorBase, colorSymbol, &verticesLength, &indicesLength);
			break;
		case BLOCK_X:
			Block_GenerateSurfaceX(size, &vertices[BLOCK_MESH_BLOCK_VERTICES], &indices[BLOCK_INDICES_STATIC_COUNT], colorBase, colorSymbol, &verticesLength, &indicesLength);
			break;
	}

	if (isGrass && character != BLOCK_GRASS_FORCED)
		self->meshGrass = Mesh_Create(vertexSolidDescription, vertexSolidDescriptionLength, vertices, sizeof(vertices[0]) * verticesLength, indices, sizeof(indices[0]) * indicesLength, MeshIndexTypeUint8);
	else
		self->meshGround = Mesh_Create(vertexSolidDescription, vertexSolidDescriptionLength, vertices, sizeof(vertices[0]) * verticesLength, indices, sizeof(indices[0]) * indicesLength, MeshIndexTypeUint8);
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
	return (character == BLOCK_GRASS || character == BLOCK_X || character == BLOCK_GRASS_FORCED);
}
static void Block_GenerateSurfaceFunc(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec2 *surface, const size_t surfaceLength, const uint8_t *dynamicIndices, const size_t dynamicIndicesLength, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength)
{
	const vec2 offset = {size[0] * 0.5f - 0.5f, size[1] * 0.5f + 0.5f};
	for (size_t i = 0; i < surfaceLength; i++) {
		vertices[i] = (VertexSolid){.position = {offset[0] + surface[i][0], 0.0f, offset[1] - surface[i][1]}, .color = VEC4_INIT_FROM(colorBase)};
		vertices[surfaceLength + i] = (VertexSolid){.position = {offset[0] + surface[i][0], 0.0f, offset[1] - surface[i][1]}, .color = VEC4_INIT_FROM(colorSymbol)};
	}
	for (size_t i = 0; i < dynamicIndicesLength; i++) {
		indices[i] = dynamicIndices[i];
	}
	if (verticesLength) *verticesLength = BLOCK_MESH_BLOCK_VERTICES + surfaceLength * 2;
	if (indicesLength) *indicesLength = BLOCK_INDICES_STATIC_COUNT + dynamicIndicesLength;
}
// '1': {{0.3125f, 0.4375f}, {0.3125f, 0.375f}, {0.5f, 0.1875f}, {0.625f, 0.1875f}, {0.625f, 0.6875f}, {0.75f, 0.6875f}, {0.75f, 0.8125f}, {0.3125f, 0.8125f}, {0.3125f, 0.6875f}, {0.4375f, 0.6875f}, {0.4375f, 0.4375f}};
static void Block_GenerateSurface1(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength)
{
	const vec2 surface[] = {{0.3125f, 0.4375f}, {0.3125f, 0.375f}, {0.5f, 0.1875f}, {0.625f, 0.1875f}, {0.625f, 0.6875f}, {0.75f, 0.6875f}, {0.75f, 0.8125f}, {0.3125f, 0.8125f}, {0.3125f, 0.6875f}, {0.4375f, 0.6875f}, {0.4375f, 0.4375f}};
	const size_t surfaceLength = sizeof(surface) / sizeof(surface[0]);
	// clang-format off
	const uint8_t dynamicIndices[] = {
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9,

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
	const size_t dynamicIndicesLength = sizeof(dynamicIndices) / sizeof(dynamicIndices[0]);
	Block_GenerateSurfaceFunc(size, vertices, indices, surface, surfaceLength, dynamicIndices, dynamicIndicesLength, colorBase, colorSymbol, verticesLength, indicesLength);
}
// '2': {{0.1875f, 0.375f}, {0.1875f, 0.25f}, {0.25f, 0.1875f}, {0.75f, 0.1875f}, {0.8125f, 0.25f}, {0.8125f, 0.5f}, {0.4375f, 0.6875f}, {0.8125f, 0.6875f}, {0.8125f, 0.8125f}, {0.1875f, 0.8125f}, {0.1875f, 0.65625f}, {0.625f, 0.4375f}, {0.625f, 0.3125f}, {0.375f, 0.3125f}, {0.375f, 0.375f}};
static void Block_GenerateSurface2(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength)
{
	const vec2 surface[] = {{0.1875f, 0.375f}, {0.1875f, 0.25f}, {0.25f, 0.1875f}, {0.75f, 0.1875f}, {0.8125f, 0.25f}, {0.8125f, 0.5f}, {0.4375f, 0.6875f}, {0.8125f, 0.6875f}, {0.8125f, 0.8125f}, {0.1875f, 0.8125f}, {0.1875f, 0.65625f}, {0.625f, 0.4375f}, {0.625f, 0.3125f}, {0.375f, 0.3125f}, {0.375f, 0.375f}};
	const size_t surfaceLength = sizeof(surface) / sizeof(surface[0]);
	// clang-format off
	const uint8_t dynamicIndices[] = {
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8,

		BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES + 14, BLOCK_MESH_BLOCK_VERTICES + 10,
		BLOCK_MESH_BLOCK_VERTICES + 14, BLOCK_MESH_BLOCK_VERTICES + 11, BLOCK_MESH_BLOCK_VERTICES + 10,
		BLOCK_MESH_BLOCK_VERTICES + 14, BLOCK_MESH_BLOCK_VERTICES + 13, BLOCK_MESH_BLOCK_VERTICES + 11,
		BLOCK_MESH_BLOCK_VERTICES + 13, BLOCK_MESH_BLOCK_VERTICES + 12, BLOCK_MESH_BLOCK_VERTICES + 11,
		BLOCK_MESH_BLOCK_VERTICES +  5, BLOCK_MESH_BLOCK_VERTICES +  7, BLOCK_MESH_BLOCK_VERTICES +  6,

		BLOCK_MESH_BLOCK_VERTICES +  9, BLOCK_MESH_BLOCK_VERTICES +  8, 20,
		BLOCK_MESH_BLOCK_VERTICES + 10, BLOCK_MESH_BLOCK_VERTICES +  9, 20,
		BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES + 10, 21,
		BLOCK_MESH_BLOCK_VERTICES +  1, BLOCK_MESH_BLOCK_VERTICES +  0, 21,
		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  1, 21,
		BLOCK_MESH_BLOCK_VERTICES +  3, BLOCK_MESH_BLOCK_VERTICES +  2, 21,
		BLOCK_MESH_BLOCK_VERTICES +  4, BLOCK_MESH_BLOCK_VERTICES +  3, 22,
		BLOCK_MESH_BLOCK_VERTICES +  5, BLOCK_MESH_BLOCK_VERTICES +  4, 22,
		BLOCK_MESH_BLOCK_VERTICES +  7, BLOCK_MESH_BLOCK_VERTICES +  5, 23,
		BLOCK_MESH_BLOCK_VERTICES +  8, BLOCK_MESH_BLOCK_VERTICES +  7, 23,

		BLOCK_MESH_BLOCK_VERTICES + 10, 20, 21,
		BLOCK_MESH_BLOCK_VERTICES +  3, 21, 22,
		BLOCK_MESH_BLOCK_VERTICES +  5, 22, 23,
		BLOCK_MESH_BLOCK_VERTICES +  8, 23, 20,
	};
	// clang-format on
	const size_t dynamicIndicesLength = sizeof(dynamicIndices) / sizeof(dynamicIndices[0]);
	Block_GenerateSurfaceFunc(size, vertices, indices, surface, surfaceLength, dynamicIndices, dynamicIndicesLength, colorBase, colorSymbol, verticesLength, indicesLength);
}
// '3': {{0.1875f, 0.1875f}, {0.75f, 0.1875f}, {0.8125f, 0.25f}, {0.8125f, 0.4375f}, {0.75f, 0.5f}, {0.8125f, 0.5625f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.1875f, 0.8125f}, {0.1875f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}, {0.375f, 0.5625f}, {0.375f, 0.4375f}, {0.625f, 0.4375f}, {0.625f, 0.3125f}, {0.1875f, 0.3125f}};
static void Block_GenerateSurface3(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength)
{
	const vec2 surface[] = {{0.1875f, 0.1875f}, {0.75f, 0.1875f}, {0.8125f, 0.25f}, {0.8125f, 0.4375f}, {0.75f, 0.5f}, {0.8125f, 0.5625f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.1875f, 0.8125f}, {0.1875f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}, {0.375f, 0.5625f}, {0.375f, 0.4375f}, {0.625f, 0.4375f}, {0.625f, 0.3125f}, {0.1875f, 0.3125f}};
	const size_t surfaceLength = sizeof(surface) / sizeof(surface[0]);
	// clang-format off
	const uint8_t dynamicIndices[] = {
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 16, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10,

		BLOCK_MESH_BLOCK_VERTICES + 16, BLOCK_MESH_BLOCK_VERTICES + 15, BLOCK_MESH_BLOCK_VERTICES + 13,
		BLOCK_MESH_BLOCK_VERTICES + 15, BLOCK_MESH_BLOCK_VERTICES + 14, BLOCK_MESH_BLOCK_VERTICES + 13,
		BLOCK_MESH_BLOCK_VERTICES + 16, BLOCK_MESH_BLOCK_VERTICES + 13, BLOCK_MESH_BLOCK_VERTICES + 12,
		BLOCK_MESH_BLOCK_VERTICES + 16, BLOCK_MESH_BLOCK_VERTICES + 12, BLOCK_MESH_BLOCK_VERTICES +  9,
		BLOCK_MESH_BLOCK_VERTICES + 12, BLOCK_MESH_BLOCK_VERTICES + 11, BLOCK_MESH_BLOCK_VERTICES + 10,
		BLOCK_MESH_BLOCK_VERTICES + 12, BLOCK_MESH_BLOCK_VERTICES + 10, BLOCK_MESH_BLOCK_VERTICES +  9,
		BLOCK_MESH_BLOCK_VERTICES +  3, BLOCK_MESH_BLOCK_VERTICES +  5, BLOCK_MESH_BLOCK_VERTICES +  4,

		BLOCK_MESH_BLOCK_VERTICES +  8, BLOCK_MESH_BLOCK_VERTICES +  7, 20,
		BLOCK_MESH_BLOCK_VERTICES +  9, BLOCK_MESH_BLOCK_VERTICES +  8, 20,
		BLOCK_MESH_BLOCK_VERTICES + 16, BLOCK_MESH_BLOCK_VERTICES +  9, 20,
		BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES + 16, 21,
		BLOCK_MESH_BLOCK_VERTICES +  1, BLOCK_MESH_BLOCK_VERTICES +  0, 21,
		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  1, 22,
		BLOCK_MESH_BLOCK_VERTICES +  3, BLOCK_MESH_BLOCK_VERTICES +  2, 22,
		BLOCK_MESH_BLOCK_VERTICES +  5, BLOCK_MESH_BLOCK_VERTICES +  3, 22,
		BLOCK_MESH_BLOCK_VERTICES +  6, BLOCK_MESH_BLOCK_VERTICES +  5, 23,
		BLOCK_MESH_BLOCK_VERTICES +  7, BLOCK_MESH_BLOCK_VERTICES +  6, 23,

		BLOCK_MESH_BLOCK_VERTICES + 16, 20, 21,
		BLOCK_MESH_BLOCK_VERTICES +  1, 21, 22,
		BLOCK_MESH_BLOCK_VERTICES +  5, 22, 23,
		BLOCK_MESH_BLOCK_VERTICES +  7, 23, 20,
	};
	// clang-format on
	const size_t dynamicIndicesLength = sizeof(dynamicIndices) / sizeof(dynamicIndices[0]);
	Block_GenerateSurfaceFunc(size, vertices, indices, surface, surfaceLength, dynamicIndices, dynamicIndicesLength, colorBase, colorSymbol, verticesLength, indicesLength);
}
// '4': {{0.1875f, 0.4375f}, {0.3125f, 0.1875f}, {0.5f, 0.1875f}, {0.375f, 0.4375f}, {0.5625f, 0.4375f}, {0.5625f, 0.1875f}, {0.75f, 0.1875f}, {0.75f, 0.4375f}, {0.8125f, 0.4375f}, {0.8125f, 0.5625f}, {0.75f, 0.5625f}, {0.75f, 0.8125f}, {0.5625f, 0.8125f}, {0.5625f, 0.5625f}, {0.1875f, 0.5625f}};
static void Block_GenerateSurface4(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength)
{
	const vec2 surface[] = {{0.1875f, 0.4375f}, {0.3125f, 0.1875f}, {0.5f, 0.1875f}, {0.375f, 0.4375f}, {0.5625f, 0.4375f}, {0.5625f, 0.1875f}, {0.75f, 0.1875f}, {0.75f, 0.4375f}, {0.8125f, 0.4375f}, {0.8125f, 0.5625f}, {0.75f, 0.5625f}, {0.75f, 0.8125f}, {0.5625f, 0.8125f}, {0.5625f, 0.5625f}, {0.1875f, 0.5625f}};
	const size_t surfaceLength = sizeof(surface) / sizeof(surface[0]);
	// clang-format off
	const uint8_t dynamicIndices[] = {
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13,

		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  5, BLOCK_MESH_BLOCK_VERTICES +  4,
		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  4, BLOCK_MESH_BLOCK_VERTICES +  3,
		BLOCK_MESH_BLOCK_VERTICES +  6, BLOCK_MESH_BLOCK_VERTICES +  8, BLOCK_MESH_BLOCK_VERTICES +  7,
		BLOCK_MESH_BLOCK_VERTICES +  9, BLOCK_MESH_BLOCK_VERTICES + 11, BLOCK_MESH_BLOCK_VERTICES + 10,
		BLOCK_MESH_BLOCK_VERTICES + 12, BLOCK_MESH_BLOCK_VERTICES + 14, BLOCK_MESH_BLOCK_VERTICES + 13,

		BLOCK_MESH_BLOCK_VERTICES + 14, BLOCK_MESH_BLOCK_VERTICES + 12, 20,
		BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES + 14, 21,
		BLOCK_MESH_BLOCK_VERTICES +  1, BLOCK_MESH_BLOCK_VERTICES +  0, 21,
		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  1, 21,
		BLOCK_MESH_BLOCK_VERTICES +  5, BLOCK_MESH_BLOCK_VERTICES +  2, 22,
		BLOCK_MESH_BLOCK_VERTICES +  6, BLOCK_MESH_BLOCK_VERTICES +  5, 22,
		BLOCK_MESH_BLOCK_VERTICES +  8, BLOCK_MESH_BLOCK_VERTICES +  6, 22,
		BLOCK_MESH_BLOCK_VERTICES +  9, BLOCK_MESH_BLOCK_VERTICES +  8, 22,
		BLOCK_MESH_BLOCK_VERTICES + 11, BLOCK_MESH_BLOCK_VERTICES +  9, 23,
		BLOCK_MESH_BLOCK_VERTICES + 12, BLOCK_MESH_BLOCK_VERTICES + 11, 23,

		BLOCK_MESH_BLOCK_VERTICES + 14, 20, 21,
		BLOCK_MESH_BLOCK_VERTICES +  2, 21, 22,
		BLOCK_MESH_BLOCK_VERTICES +  9, 22, 23,
		BLOCK_MESH_BLOCK_VERTICES + 12, 23, 20,
	};
	// clang-format on
	const size_t dynamicIndicesLength = sizeof(dynamicIndices) / sizeof(dynamicIndices[0]);
	Block_GenerateSurfaceFunc(size, vertices, indices, surface, surfaceLength, dynamicIndices, dynamicIndicesLength, colorBase, colorSymbol, verticesLength, indicesLength);
}
// '5': {{0.1875f, 0.1875f}, {0.8125f, 0.1875f}, {0.8125f, 0.3125f}, {0.375f, 0.3125f}, {0.375f, 0.4375f}, {0.75f, 0.4375f}, {0.8125f, 0.5f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.1875f, 0.8125f}, {0.1875f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}, {0.1875f, 0.5625f}};
static void Block_GenerateSurface5(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength)
{
	const vec2 surface[] = {{0.1875f, 0.1875f}, {0.8125f, 0.1875f}, {0.8125f, 0.3125f}, {0.375f, 0.3125f}, {0.375f, 0.4375f}, {0.75f, 0.4375f}, {0.8125f, 0.5f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.1875f, 0.8125f}, {0.1875f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}, {0.1875f, 0.5625f}};
	const size_t surfaceLength = sizeof(surface) / sizeof(surface[0]);
	// clang-format off
	const uint8_t dynamicIndices[] = {
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10,

		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  5, BLOCK_MESH_BLOCK_VERTICES +  3,
		BLOCK_MESH_BLOCK_VERTICES +  3, BLOCK_MESH_BLOCK_VERTICES +  5, BLOCK_MESH_BLOCK_VERTICES +  4,
		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  6, BLOCK_MESH_BLOCK_VERTICES +  5,
		BLOCK_MESH_BLOCK_VERTICES + 11, BLOCK_MESH_BLOCK_VERTICES + 13, BLOCK_MESH_BLOCK_VERTICES + 12,
		BLOCK_MESH_BLOCK_VERTICES + 10, BLOCK_MESH_BLOCK_VERTICES + 13, BLOCK_MESH_BLOCK_VERTICES + 11,

		BLOCK_MESH_BLOCK_VERTICES +  9, BLOCK_MESH_BLOCK_VERTICES +  8, 20,
		BLOCK_MESH_BLOCK_VERTICES + 10, BLOCK_MESH_BLOCK_VERTICES +  9, 20,
		BLOCK_MESH_BLOCK_VERTICES + 13, BLOCK_MESH_BLOCK_VERTICES + 10, 20,
		BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES + 13, 21,
		BLOCK_MESH_BLOCK_VERTICES +  1, BLOCK_MESH_BLOCK_VERTICES +  0, 21,
		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  1, 22,
		BLOCK_MESH_BLOCK_VERTICES +  6, BLOCK_MESH_BLOCK_VERTICES +  2, 22,
		BLOCK_MESH_BLOCK_VERTICES +  7, BLOCK_MESH_BLOCK_VERTICES +  6, 23,
		BLOCK_MESH_BLOCK_VERTICES +  8, BLOCK_MESH_BLOCK_VERTICES +  7, 23,

		BLOCK_MESH_BLOCK_VERTICES + 13, 20, 21,
		BLOCK_MESH_BLOCK_VERTICES +  1, 21, 22,
		BLOCK_MESH_BLOCK_VERTICES +  6, 22, 23,
		BLOCK_MESH_BLOCK_VERTICES +  8, 23, 20,
	};
	// clang-format on
	const size_t dynamicIndicesLength = sizeof(dynamicIndices) / sizeof(dynamicIndices[0]);
	Block_GenerateSurfaceFunc(size, vertices, indices, surface, surfaceLength, dynamicIndices, dynamicIndicesLength, colorBase, colorSymbol, verticesLength, indicesLength);
}
// '6': {{0.25f, 0.1875f}, {0.8125f, 0.1875f}, {0.8125f, 0.3125f}, {0.375f, 0.3125f}, {0.375f, 0.4375f}, {0.75f, 0.4375f}, {0.8125f, 0.5f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.25f, 0.8125f}, {0.1875f, 0.75f}, {0.1875f, 0.25f}, {0.375f, 0.5625f}, {0.375f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}};
static void Block_GenerateSurface6(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength)
{
	const vec2 surface[] = {{0.25f, 0.1875f}, {0.8125f, 0.1875f}, {0.8125f, 0.3125f}, {0.375f, 0.3125f}, {0.375f, 0.4375f}, {0.75f, 0.4375f}, {0.8125f, 0.5f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.25f, 0.8125f}, {0.1875f, 0.75f}, {0.1875f, 0.25f}, {0.375f, 0.5625f}, {0.375f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}};
	const size_t surfaceLength = sizeof(surface) / sizeof(surface[0]);
	// clang-format off
	const uint8_t dynamicIndices[] = {
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4,

		BLOCK_MESH_BLOCK_VERTICES +  3, BLOCK_MESH_BLOCK_VERTICES +  5, BLOCK_MESH_BLOCK_VERTICES +  4,
		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  5, BLOCK_MESH_BLOCK_VERTICES +  3,
		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  6, BLOCK_MESH_BLOCK_VERTICES +  5,
		BLOCK_MESH_BLOCK_VERTICES + 12, BLOCK_MESH_BLOCK_VERTICES + 14, BLOCK_MESH_BLOCK_VERTICES + 13,
		BLOCK_MESH_BLOCK_VERTICES + 12, BLOCK_MESH_BLOCK_VERTICES + 15, BLOCK_MESH_BLOCK_VERTICES + 14,

		BLOCK_MESH_BLOCK_VERTICES +  9, BLOCK_MESH_BLOCK_VERTICES +  8, 20,
		BLOCK_MESH_BLOCK_VERTICES + 10, BLOCK_MESH_BLOCK_VERTICES +  9, 20,
		BLOCK_MESH_BLOCK_VERTICES + 11, BLOCK_MESH_BLOCK_VERTICES + 10, 21,
		BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES + 11, 21,
		BLOCK_MESH_BLOCK_VERTICES +  1, BLOCK_MESH_BLOCK_VERTICES +  0, 22,
		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  1, 22,
		BLOCK_MESH_BLOCK_VERTICES +  6, BLOCK_MESH_BLOCK_VERTICES +  2, 22,
		BLOCK_MESH_BLOCK_VERTICES +  7, BLOCK_MESH_BLOCK_VERTICES +  6, 23,
		BLOCK_MESH_BLOCK_VERTICES +  8, BLOCK_MESH_BLOCK_VERTICES +  7, 23,

		BLOCK_MESH_BLOCK_VERTICES + 10, 20, 21,
		BLOCK_MESH_BLOCK_VERTICES +  0, 21, 22,
		BLOCK_MESH_BLOCK_VERTICES +  6, 22, 23,
		BLOCK_MESH_BLOCK_VERTICES +  8, 23, 20,
	};
	// clang-format on
	const size_t dynamicIndicesLength = sizeof(dynamicIndices) / sizeof(dynamicIndices[0]);
	Block_GenerateSurfaceFunc(size, vertices, indices, surface, surfaceLength, dynamicIndices, dynamicIndicesLength, colorBase, colorSymbol, verticesLength, indicesLength);
}
// '7': {{0.1875f, 0.1875f}, {0.8125f, 0.1875f}, {0.8125f, 0.3125f}, {0.5625f, 0.8125f}, {0.375f, 0.8125f}, {0.625f, 0.3125f}, {0.1875f, 0.3125f}};
static void Block_GenerateSurface7(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength)
{
	const vec2 surface[] = {{0.1875f, 0.1875f}, {0.8125f, 0.1875f}, {0.8125f, 0.3125f}, {0.5625f, 0.8125f}, {0.375f, 0.8125f}, {0.625f, 0.3125f}, {0.1875f, 0.3125f}};
	const size_t surfaceLength = sizeof(surface) / sizeof(surface[0]);
	// clang-format off
	const uint8_t dynamicIndices[] = {
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 5,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 6,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 5,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 3, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 4,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 5,

		BLOCK_MESH_BLOCK_VERTICES + 4, BLOCK_MESH_BLOCK_VERTICES + 6, BLOCK_MESH_BLOCK_VERTICES + 5,

		BLOCK_MESH_BLOCK_VERTICES + 6, BLOCK_MESH_BLOCK_VERTICES + 4, 20,
		BLOCK_MESH_BLOCK_VERTICES + 0, BLOCK_MESH_BLOCK_VERTICES + 6, 21,
		BLOCK_MESH_BLOCK_VERTICES + 1, BLOCK_MESH_BLOCK_VERTICES + 0, 21,
		BLOCK_MESH_BLOCK_VERTICES + 2, BLOCK_MESH_BLOCK_VERTICES + 1, 22,
		BLOCK_MESH_BLOCK_VERTICES + 3, BLOCK_MESH_BLOCK_VERTICES + 2, 23,
		BLOCK_MESH_BLOCK_VERTICES + 4, BLOCK_MESH_BLOCK_VERTICES + 3, 23,

		BLOCK_MESH_BLOCK_VERTICES + 6, 20, 21,
		BLOCK_MESH_BLOCK_VERTICES + 1, 21, 22,
		BLOCK_MESH_BLOCK_VERTICES + 2, 22, 23,
		BLOCK_MESH_BLOCK_VERTICES + 4, 23, 20,
	};
	// clang-format on
	const size_t dynamicIndicesLength = sizeof(dynamicIndices) / sizeof(dynamicIndices[0]);
	Block_GenerateSurfaceFunc(size, vertices, indices, surface, surfaceLength, dynamicIndices, dynamicIndicesLength, colorBase, colorSymbol, verticesLength, indicesLength);
}
// '8': {{0.1875f, 0.25f}, {0.25f, 0.1875f}, {0.75f, 0.1875f}, {0.8125f, 0.25f}, {0.8125f, 0.4375f}, {0.75f, 0.5f}, {0.8125f, 0.5625f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.25f, 0.8125f}, {0.1875f, 0.75f}, {0.1875f, 0.5625f}, {0.25f, 0.5f}, {0.1875f, 0.4375f}, {0.375f, 0.3125f}, {0.375f, 0.4375f}, {0.625f, 0.4375f}, {0.625f, 0.3125f}, {0.375f, 0.5625f}, {0.375f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}};
static void Block_GenerateSurface8(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength)
{
	const vec2 surface[] = {{0.1875f, 0.25f}, {0.25f, 0.1875f}, {0.75f, 0.1875f}, {0.8125f, 0.25f}, {0.8125f, 0.4375f}, {0.75f, 0.5f}, {0.8125f, 0.5625f}, {0.8125f, 0.75f}, {0.75f, 0.8125f}, {0.25f, 0.8125f}, {0.1875f, 0.75f}, {0.1875f, 0.5625f}, {0.25f, 0.5f}, {0.1875f, 0.4375f}, {0.375f, 0.3125f}, {0.375f, 0.4375f}, {0.625f, 0.4375f}, {0.625f, 0.3125f}, {0.375f, 0.5625f}, {0.375f, 0.6875f}, {0.625f, 0.6875f}, {0.625f, 0.5625f}};
	const size_t surfaceLength = sizeof(surface) / sizeof(surface[0]);
	// clang-format off
	const uint8_t dynamicIndices[] = {
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 17,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 20,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 19,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 16,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 21, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 16,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 21,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 18,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 18,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 12, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 17, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 17,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 16, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 17,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 21,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 20, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 21,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 20,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 19, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 20,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 19,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 18, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 19,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 13, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 14, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 15, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 16, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 18,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 16, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 21, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 18,

		BLOCK_MESH_BLOCK_VERTICES + 14, BLOCK_MESH_BLOCK_VERTICES + 16, BLOCK_MESH_BLOCK_VERTICES + 15,
		BLOCK_MESH_BLOCK_VERTICES + 14, BLOCK_MESH_BLOCK_VERTICES + 17, BLOCK_MESH_BLOCK_VERTICES + 16,
		BLOCK_MESH_BLOCK_VERTICES + 18, BLOCK_MESH_BLOCK_VERTICES + 20, BLOCK_MESH_BLOCK_VERTICES + 19,
		BLOCK_MESH_BLOCK_VERTICES + 18, BLOCK_MESH_BLOCK_VERTICES + 21, BLOCK_MESH_BLOCK_VERTICES + 20,
		BLOCK_MESH_BLOCK_VERTICES +  4, BLOCK_MESH_BLOCK_VERTICES +  6, BLOCK_MESH_BLOCK_VERTICES +  5,
		BLOCK_MESH_BLOCK_VERTICES + 11, BLOCK_MESH_BLOCK_VERTICES + 13, BLOCK_MESH_BLOCK_VERTICES + 12,

		BLOCK_MESH_BLOCK_VERTICES +  9, BLOCK_MESH_BLOCK_VERTICES +  8, 20,
		BLOCK_MESH_BLOCK_VERTICES + 10, BLOCK_MESH_BLOCK_VERTICES +  9, 20,
		BLOCK_MESH_BLOCK_VERTICES + 11, BLOCK_MESH_BLOCK_VERTICES + 10, 21,
		BLOCK_MESH_BLOCK_VERTICES + 13, BLOCK_MESH_BLOCK_VERTICES + 11, 21,
		BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES + 13, 21,
		BLOCK_MESH_BLOCK_VERTICES +  1, BLOCK_MESH_BLOCK_VERTICES +  0, 21,
		BLOCK_MESH_BLOCK_VERTICES +  2, BLOCK_MESH_BLOCK_VERTICES +  1, 22,
		BLOCK_MESH_BLOCK_VERTICES +  3, BLOCK_MESH_BLOCK_VERTICES +  2, 22,
		BLOCK_MESH_BLOCK_VERTICES +  4, BLOCK_MESH_BLOCK_VERTICES +  3, 23,
		BLOCK_MESH_BLOCK_VERTICES +  6, BLOCK_MESH_BLOCK_VERTICES +  4, 23,
		BLOCK_MESH_BLOCK_VERTICES +  7, BLOCK_MESH_BLOCK_VERTICES +  6, 23,
		BLOCK_MESH_BLOCK_VERTICES +  8, BLOCK_MESH_BLOCK_VERTICES +  7, 23,

		BLOCK_MESH_BLOCK_VERTICES + 10, 20, 21,
		BLOCK_MESH_BLOCK_VERTICES +  1, 21, 22,
		BLOCK_MESH_BLOCK_VERTICES +  3, 22, 23,
		BLOCK_MESH_BLOCK_VERTICES +  8, 23, 20,
	};
	// clang-format on
	const size_t dynamicIndicesLength = sizeof(dynamicIndices) / sizeof(dynamicIndices[0]);
	Block_GenerateSurfaceFunc(size, vertices, indices, surface, surfaceLength, dynamicIndices, dynamicIndicesLength, colorBase, colorSymbol, verticesLength, indicesLength);
}
// 'x': {{0.1875f, 0.3125f}, {0.3125f, 0.1875f}, {0.5f, 0.375f}, {0.6875f, 0.1875f}, {0.8125f, 0.3125f}, {0.625f, 0.5f}, {0.8125f, 0.6875f}, {0.6875f, 0.8125f}, {0.5f, 0.625f}, {0.3125f, 0.8125f}, {0.1875f, 0.6875f}, {0.375f, 0.5f}};
static void Block_GenerateSurfaceX(const vec2 size, VertexSolid *vertices, uint8_t *indices, const vec4 colorBase, const vec4 colorSymbol, size_t *verticesLength, size_t *indicesLength)
{
	const vec2 surface[] = {{0.1875f, 0.3125f}, {0.3125f, 0.1875f}, {0.5f, 0.375f}, {0.6875f, 0.1875f}, {0.8125f, 0.3125f}, {0.625f, 0.5f}, {0.8125f, 0.6875f}, {0.6875f, 0.8125f}, {0.5f, 0.625f}, {0.3125f, 0.8125f}, {0.1875f, 0.6875f}, {0.375f, 0.5f}};
	const size_t surfaceLength = sizeof(surface) / sizeof(surface[0]);
	// clang-format off
	const uint8_t dynamicIndices[] = {
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  1, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  0, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  3, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  4, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  6, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  7, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  9, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 10, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  5, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8,
		surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  2, surfaceLength + BLOCK_MESH_BLOCK_VERTICES +  8, surfaceLength + BLOCK_MESH_BLOCK_VERTICES + 11,

		BLOCK_MESH_BLOCK_VERTICES +  1, BLOCK_MESH_BLOCK_VERTICES +  3, BLOCK_MESH_BLOCK_VERTICES +  2,
		BLOCK_MESH_BLOCK_VERTICES +  4, BLOCK_MESH_BLOCK_VERTICES +  6, BLOCK_MESH_BLOCK_VERTICES +  5,
		BLOCK_MESH_BLOCK_VERTICES +  7, BLOCK_MESH_BLOCK_VERTICES +  9, BLOCK_MESH_BLOCK_VERTICES +  8,
		BLOCK_MESH_BLOCK_VERTICES + 10, BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES + 11,

		BLOCK_MESH_BLOCK_VERTICES + 10, BLOCK_MESH_BLOCK_VERTICES +  9, 20,
		BLOCK_MESH_BLOCK_VERTICES +  0, BLOCK_MESH_BLOCK_VERTICES + 10, 20,
		BLOCK_MESH_BLOCK_VERTICES +  1, BLOCK_MESH_BLOCK_VERTICES +  0, 21,
		BLOCK_MESH_BLOCK_VERTICES +  3, BLOCK_MESH_BLOCK_VERTICES +  1, 21,
		BLOCK_MESH_BLOCK_VERTICES +  4, BLOCK_MESH_BLOCK_VERTICES +  3, 22,
		BLOCK_MESH_BLOCK_VERTICES +  6, BLOCK_MESH_BLOCK_VERTICES +  4, 22,
		BLOCK_MESH_BLOCK_VERTICES +  7, BLOCK_MESH_BLOCK_VERTICES +  6, 23,
		BLOCK_MESH_BLOCK_VERTICES +  9, BLOCK_MESH_BLOCK_VERTICES +  7, 23,

		BLOCK_MESH_BLOCK_VERTICES + 0, 20, 21,
		BLOCK_MESH_BLOCK_VERTICES + 3, 21, 22,
		BLOCK_MESH_BLOCK_VERTICES + 6, 22, 23,
		BLOCK_MESH_BLOCK_VERTICES + 9, 23, 20,
	};
	// clang-format on
	const size_t dynamicIndicesLength = sizeof(dynamicIndices) / sizeof(dynamicIndices[0]);
	Block_GenerateSurfaceFunc(size, vertices, indices, surface, surfaceLength, dynamicIndices, dynamicIndicesLength, colorBase, colorSymbol, verticesLength, indicesLength);
}
