#ifndef __RENDER_TYPE_VERTEX_H__
#define __RENDER_TYPE_VERTEX_H__

#include "cglm/cglm.h" // IWYU pragma: export
#include "render/mesh.h"
#include "render/shader.h"
#include <stddef.h>

typedef struct VertexSolid {
	vec3 position;
	vec4 color;
} VertexSolid;
typedef struct VertexTextured {
	vec3 position;
	vec2 textureCoord;
} VertexTextured;
typedef struct VertexTexturedColored {
	vec3 position;
	vec2 textureCoord;
	vec4 color;
} VertexTexturedColored;
#define VERTEX_QUADRATIC_TYPE_CONVEX (1.0f)
#define VERTEX_QUADRATIC_TYPE_CONCAVE (-1.0f)
typedef struct VertexQuadratic {
	vec3 position;
	vec2 uv;
	vec4 color;
	float type;
} VertexQuadratic;
enum VertexSolidAttributesLocation {
	VertexSolidAttributesLocationPosition = 0,
	VertexSolidAttributesLocationColor = 1,
};
enum VertexTexturedAttributesLocation {
	VertexTexturedAttributesLocationPosition = 0,
	VertexTexturedAttributesLocationTextureCoord = 1,
};
enum VertexTexturedColoredAttributesLocation {
	VertexTexturedColoredAttributesLocationPosition = 0,
	VertexTexturedColoredAttributesLocationTextureCoord = 1,
	VertexTexturedColoredAttributesLocationColor = 2,
};
enum VertexQuadraticAttributesLocation {
	VertexQuadraticAttributesLocationPosition = 0,
	VertexQuadraticAttributesLocationCurveUV = 1,
	VertexQuadraticAttributesLocationColor = 2,
	VertexQuadraticAttributesLocationType = 3,
};

static const MeshVertexAttributeDescription vertexSolidDescription[] = {
	(MeshVertexAttributeDescription){.location = VertexSolidAttributesLocationPosition, .size = 3, .stride = sizeof(VertexSolid), .offset = (const void *)offsetof(VertexSolid, position)},
	(MeshVertexAttributeDescription){.location = VertexSolidAttributesLocationColor, .size = 4, .stride = sizeof(VertexSolid), .offset = (const void *)offsetof(VertexSolid, color)},
};
static const size_t vertexSolidDescriptionLength = sizeof(vertexSolidDescription) / sizeof(MeshVertexAttributeDescription);
static const MeshVertexAttributeDescription vertexTexturedDescription[] = {
	(MeshVertexAttributeDescription){.location = VertexTexturedAttributesLocationPosition, .size = 3, .stride = sizeof(VertexTextured), .offset = (const void *)offsetof(VertexTextured, position)},
	(MeshVertexAttributeDescription){.location = VertexTexturedAttributesLocationTextureCoord, .size = 2, .stride = sizeof(VertexTextured), .offset = (const void *)offsetof(VertexTextured, textureCoord)},
};
static const size_t vertexTexturedDescriptionLength = sizeof(vertexTexturedDescription) / sizeof(MeshVertexAttributeDescription);
static const MeshVertexAttributeDescription vertexTexturedColoredDescription[] = {
	(MeshVertexAttributeDescription){.location = VertexTexturedColoredAttributesLocationPosition, .size = 3, .stride = sizeof(VertexTexturedColored), .offset = (const void *)offsetof(VertexTexturedColored, position)},
	(MeshVertexAttributeDescription){.location = VertexTexturedColoredAttributesLocationTextureCoord, .size = 2, .stride = sizeof(VertexTexturedColored), .offset = (const void *)offsetof(VertexTexturedColored, textureCoord)},
	(MeshVertexAttributeDescription){.location = VertexTexturedColoredAttributesLocationColor, .size = 4, .stride = sizeof(VertexTexturedColored), .offset = (const void *)offsetof(VertexTexturedColored, color)},
};
static const size_t vertexTexturedColoredDescriptionLength = sizeof(vertexTexturedColoredDescription) / sizeof(MeshVertexAttributeDescription);
static const MeshVertexAttributeDescription vertexQuadraticDescription[] = {
	(MeshVertexAttributeDescription){.location = VertexQuadraticAttributesLocationPosition, .size = 3, .stride = sizeof(VertexQuadratic), .offset = (const void *)offsetof(VertexQuadratic, position)},
	(MeshVertexAttributeDescription){.location = VertexQuadraticAttributesLocationCurveUV, .size = 2, .stride = sizeof(VertexQuadratic), .offset = (const void *)offsetof(VertexQuadratic, uv)},
	(MeshVertexAttributeDescription){.location = VertexQuadraticAttributesLocationColor, .size = 4, .stride = sizeof(VertexQuadratic), .offset = (const void *)offsetof(VertexQuadratic, color)},
	(MeshVertexAttributeDescription){.location = VertexQuadraticAttributesLocationType, .size = 1, .stride = sizeof(VertexQuadratic), .offset = (const void *)offsetof(VertexQuadratic, type)},
};
static const size_t vertexQuadraticDescriptionLength = sizeof(vertexQuadraticDescription) / sizeof(MeshVertexAttributeDescription);

static const AttribLocation vertexSolidAttributeLocations[] = {
	(AttribLocation){.name = "inPos", .location = VertexSolidAttributesLocationPosition},
	(AttribLocation){.name = "inColor", .location = VertexSolidAttributesLocationColor},
};
static const size_t vertexSolidAttributeLocationsLength = sizeof(vertexSolidAttributeLocations) / sizeof(AttribLocation);
static const AttribLocation vertexTexturedAttributeLocations[] = {
	(AttribLocation){.name = "inPos", .location = VertexTexturedAttributesLocationPosition},
	(AttribLocation){.name = "inTexCoord", .location = VertexTexturedAttributesLocationTextureCoord},
};
static const size_t vertexTexturedAttributeLocationsLength = sizeof(vertexTexturedAttributeLocations) / sizeof(AttribLocation);
static const AttribLocation vertexTexturedColoredAttributeLocations[] = {
	(AttribLocation){.name = "inPos", .location = VertexTexturedColoredAttributesLocationPosition},
	(AttribLocation){.name = "inTexCoord", .location = VertexTexturedColoredAttributesLocationTextureCoord},
	(AttribLocation){.name = "inColor", .location = VertexTexturedColoredAttributesLocationColor},
};
static const size_t vertexTexturedColoredAttributeLocationsLength = sizeof(vertexTexturedColoredAttributeLocations) / sizeof(AttribLocation);
static const AttribLocation vertexQuadraticAttributeLocations[] = {
	(AttribLocation){.name = "inPos", .location = VertexQuadraticAttributesLocationPosition},
	(AttribLocation){.name = "inCurveUV", .location = VertexQuadraticAttributesLocationCurveUV},
	(AttribLocation){.name = "inColor", .location = VertexQuadraticAttributesLocationColor},
	(AttribLocation){.name = "inType", .location = VertexQuadraticAttributesLocationType},
};
static const size_t vertexQuadraticAttributeLocationsLength = sizeof(vertexQuadraticAttributeLocations) / sizeof(AttribLocation);

#endif
