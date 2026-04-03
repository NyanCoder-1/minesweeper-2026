#include "render/mesh.h"
#include <stddef.h>

Mesh Mesh_Create(const MeshVertexAttributeDescription *attributes, int attributeCount, const void *vertices, uint64_t verticesDataSize, const void *indices, uint64_t indicesDataSize, uint8_t indexType)
{
	GLuint vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, verticesDataSize, vertices, GL_STATIC_DRAW);

	GLuint element_buffer;
	glGenBuffers(1, &element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesDataSize, indices, GL_STATIC_DRAW);

	const size_t indicesCount = indicesDataSize / (indexType == MeshIndexTypeUint8 ? sizeof(uint8_t) : indexType == MeshIndexTypeUint16 ? sizeof(uint16_t)
																																		: sizeof(uint32_t));

	for (int i = 0; i < attributeCount; i++) {
		const MeshVertexAttributeDescription attribute = attributes[i];
		glEnableVertexAttribArray(attribute.location);
		glVertexAttribPointer(attribute.location, attribute.size, GL_FLOAT, GL_FALSE, attribute.stride, attribute.offset);
	}

	glBindVertexArray(0);

	return (Mesh){.vao = vertex_array, .vbo = vertex_buffer, .ebo = element_buffer, .indicesCount = indicesCount, .indexType = indexType};
}
void Mesh_Destroy(Mesh *mesh)
{
	if (!mesh || !mesh->vao)
		return;

	glBindVertexArray(0);
	glDeleteBuffers(2, (GLuint[]){mesh->vbo, mesh->ebo});
	glDeleteVertexArrays(1, &mesh->vao);
}
void Mesh_Update(Mesh *self, const void *vertices, uint64_t verticesDataSize, const void *indices, uint64_t indicesDataSize, uint8_t indexType)
{
	glBindVertexArray(self->vao);
	glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
	glBufferData(GL_ARRAY_BUFFER, verticesDataSize, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesDataSize, indices, GL_STATIC_DRAW);
	self->indexType = indexType;
	glBindVertexArray(0);

	const size_t indicesCount = indicesDataSize / (indexType == MeshIndexTypeUint8 ? sizeof(uint8_t) : indexType == MeshIndexTypeUint16 ? sizeof(uint16_t)
																																		: sizeof(uint32_t));
	self->indicesCount = indicesCount;
}
void Mesh_UpdateVertices(const Mesh self, const void *vertices, uint64_t verticesDataSize)
{
	glBindVertexArray(self.vao);
	glBindBuffer(GL_ARRAY_BUFFER, self.vbo);
	glBufferData(GL_ARRAY_BUFFER, verticesDataSize, vertices, GL_STATIC_DRAW);
	glBindVertexArray(0);
}
void Mesh_Render(const Mesh self)
{
	glBindVertexArray(self.vao);
	glDrawElements(GL_TRIANGLES, self.indicesCount, self.indexType == MeshIndexTypeUint8 ? GL_UNSIGNED_BYTE : self.indexType == MeshIndexTypeUint16 ? GL_UNSIGNED_SHORT
																																					: GL_UNSIGNED_INT,
				   0);
}
void Mesh_RenderTopology(const Mesh self, GLenum topology)
{
	glBindVertexArray(self.vao);
	glDrawElements(topology, self.indicesCount, self.indexType == MeshIndexTypeUint8 ? GL_UNSIGNED_BYTE : self.indexType == MeshIndexTypeUint16 ? GL_UNSIGNED_SHORT
																																				: GL_UNSIGNED_INT,
				   0);
}