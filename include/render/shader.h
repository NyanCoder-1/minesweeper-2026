#ifndef __RENDER_SHADER_H__
#define __RENDER_SHADER_H__

#include "cglm/cglm.h" // IWYU pragma: export
#include <GLES3/gl3.h>
typedef struct AttribLocation {
	const char *name;
	GLuint location;
} AttribLocation;
typedef struct UniformLocation {
	const char *name;
	GLuint location;
} UniformLocation;
typedef struct Shader {
	GLuint program;
	GLuint locationMVP;
} Shader;
Shader Shader_Create(const char *vertexShaderText, const char *fragmentShaderText, const AttribLocation *attribLocations, int attribCount);
void Shader_Destroy(const Shader shader);
void Shader_SetMVP(const Shader shader, const mat4 mvp);
void Shader_Apply(const Shader shader);
// This for material parameters
void Shader_GetUniformLocations(const Shader shader, UniformLocation *uniformLocations, int uniformCount);

#endif
