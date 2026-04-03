#ifndef __RENDER_TEXTURE_H__
#define __RENDER_TEXTURE_H__

typedef struct Texture Texture;

#include <GLES3/gl3.h>
#include <stddef.h>
#include <stdint.h>

struct Texture {
	GLuint id;
};

Texture Texture_LoadFromFile(const char *path);
Texture Texture_LoadFromMemory(const void *data, size_t size);
/**
 * Creates a texture from a memory
 * @param data The raw pixel data of the texture.
 * @param format The pixel format of the texture (GL_RED, GL_RG, GL_RGB, and GL_RGBA).
 */
Texture Texture_Create(const void *data, GLsizei width, GLsizei height, GLenum format);
void Texture_Destroy(Texture *texture);
void Texture_Update(const Texture self, const void *data, GLsizei width, GLsizei height, GLenum format);
void Texture_Update_LoadFromFile(const Texture self, const char *path);
void Texture_Update_LoadFromMemory(const Texture self, const void *data, size_t size);
void Texture_Bind(const Texture self);

#endif
