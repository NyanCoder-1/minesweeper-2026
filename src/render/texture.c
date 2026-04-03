#include "render/texture.h"
#include "stb_image.h"
#include "utility/asset-loader.h"
#include "utility/function-name.h"
#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Texture Texture_LoadFromFile(const char *path)
{
	size_t size = 0;
	void *asset = assetLoad(path, &size);

	// Create a texture from the memory data
	Texture result = Texture_LoadFromMemory(asset, size);

	// Free the allocated memory
	free(asset);

	return result;
}
Texture Texture_LoadFromMemory(const void *data, size_t size)
{
	int width = 256;
	int height = 256;
	int channels = 0;
	stbi_uc *image = stbi_load_from_memory(data, size, &width, &height, &channels, 0);
	if (image == NULL) {
		fprintf(stderr, "%s: Failed to load image\n", __FUNCTION_NAME__);
		return (Texture){0};
	}
	GLenum format = GL_RGBA;
	switch (channels) {
	case 1:
		format = GL_RED;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		fprintf(stderr, "%s: Unsupported number of channels\n", __FUNCTION_NAME__);
		return (Texture){0};
	}
	Texture result = Texture_Create(image, (GLsizei)width, (GLsizei)height, format);
	stbi_image_free(image);
	return result;
}
Texture Texture_Create(const void *data, GLsizei width, GLsizei height, GLenum format)
{
	GLint internalFormat = GL_RGBA8;
	switch (format) {
	case GL_RED:
		internalFormat = GL_R8;
		break;
	case GL_RG:
		internalFormat = GL_RG8;
		break;
	case GL_RGB:
		internalFormat = GL_RGB8;
		break;
	case GL_RGBA:
		internalFormat = GL_RGBA8;
		break;
	default:
		fprintf(stderr, "%s: Unsupported pixel size\n", __func__);
		return (Texture){0};
	}

	GLint previousBindedTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousBindedTexture);
	Texture result;
	glGenTextures(1, &result.id);
	glBindTexture(GL_TEXTURE_2D, result.id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, previousBindedTexture);

	return result;
}
void Texture_Destroy(Texture *texture)
{
	if (!texture || !texture->id)
		return;
	glDeleteTextures(1, (GLuint[1]){texture->id});
	memset(texture, 0, sizeof(Texture));
}
void Texture_Update(const Texture self, const void *data, GLsizei width, GLsizei height, GLenum format)
{
	GLint internalFormat = GL_RGBA8;
	switch (format) {
	case GL_RED:
		internalFormat = GL_R8;
		break;
	case GL_RG:
		internalFormat = GL_RG8;
		break;
	case GL_RGB:
		internalFormat = GL_RGB8;
		break;
	case GL_RGBA:
		internalFormat = GL_RGBA8;
		break;
	default:
		fprintf(stderr, "%s: Unsupported pixel size\n", __func__);
		return;
	}

	GLint previousBindedTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousBindedTexture);

	glBindTexture(GL_TEXTURE_2D, self.id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, previousBindedTexture);
}
void Texture_Update_LoadFromFile(const Texture self, const char *path)
{
	FILE *file = fopen(path, "rb");
	if (!file) {
		fprintf(stderr, "%s: Failed to open file \"%s\"\n", __FUNCTION_NAME__, path);
		return;
	}

	// Calculate the size of the file
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Read the whole file into memory
	static const size_t nullTerminationSize = sizeof(char); // For the null terminator
	void *asset = malloc(size + nullTerminationSize);
	uint8_t *string = (uint8_t *)asset;
	fread(string, size, 1, file);
	fclose(file);

	// Null-terminate the string
	string[size] = 0;

	// Create a texture from the memory data
	Texture_Update_LoadFromMemory(self, asset, size);

	// Free the allocated memory
	free(asset);
}
void Texture_Update_LoadFromMemory(const Texture self, const void *data, size_t size)
{
	int width = 256;
	int height = 256;
	int channels = 0;
	stbi_uc *image = stbi_load_from_memory(data, size, &width, &height, &channels, 0);
	if (image == NULL) {
		fprintf(stderr, "%s: Failed to load image\n", __FUNCTION_NAME__);
		return;
	}
	GLenum format = GL_RGBA;
	switch (channels) {
	case 1:
		format = GL_RED;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		fprintf(stderr, "%s: Unsupported number of channels\n", __FUNCTION_NAME__);
		return;
	}
	Texture_Update(self, image, (GLsizei)width, (GLsizei)height, format);
	stbi_image_free(image);
}
void Texture_Bind(const Texture self)
{
	glBindTexture(GL_TEXTURE_2D, self.id);
}
