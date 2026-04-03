#ifndef __RENDER_MESH_H__
#define __RENDER_MESH_H__

#include <GLES3/gl3.h>
typedef struct MeshVertexAttributeDescription {
	GLuint location;
	GLint size;
	GLsizei stride;
	const void *offset;
} MeshVertexAttributeDescription;

enum MeshIndexType {
	MeshIndexTypeUint8 = 0,
	MeshIndexTypeUint16 = 1,
	MeshIndexTypeUint32 = 2
};
typedef struct Mesh {
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	uint64_t indicesCount;
	uint8_t indexType;
} Mesh;

Mesh Mesh_Create(const MeshVertexAttributeDescription *attributes, int attributeCount, const void *vertices, uint64_t verticesDataSize, const void *indices, uint64_t indicesDataSize, uint8_t indexType);
void Mesh_Destroy(Mesh *mesh);
void Mesh_Update(Mesh *self, const void *vertices, uint64_t verticesDataSize, const void *indices, uint64_t indicesDataSize, uint8_t indexType);
void Mesh_UpdateVertices(const Mesh self, const void *vertices, uint64_t verticesDataSize);
void Mesh_Render(const Mesh self);
void Mesh_RenderTopology(const Mesh self, GLenum topology);

#endif
