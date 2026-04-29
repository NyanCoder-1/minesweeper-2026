#include "render/shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Shader Shader_Create(const char *vertexShaderText, const char *fragmentShaderText, const AttribLocation *attribLocations, int attribCount)
{
	const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertexShaderText, NULL);
	glCompileShader(vertex_shader);
	GLint isCompiled = 0;
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);
		char *infoLog = malloc(maxLength);
		glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, &infoLog[0]);

		fprintf(stdout, "Vertex Shader Compilation Failed:\n%s\n", infoLog);
		free(infoLog);
	}

	const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragmentShaderText, NULL);
	glCompileShader(fragment_shader);
	isCompiled = 0;
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);
		char *infoLog = malloc(maxLength);
		glGetShaderInfoLog(fragment_shader, maxLength, &maxLength, &infoLog[0]);

		fprintf(stdout, "Vertex Shader Compilation Failed:\n%s\n", infoLog);
		free(infoLog);
	}

	const GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	for (int i = 0; i < attribCount; i++) {
		const AttribLocation *attribLocation = &attribLocations[i];
		glBindAttribLocation(program, attribLocation->location, attribLocation->name);
	}
	glLinkProgram(program);

	// Predelete shader objects, so they automatically delete when the program is deleted
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	GLuint uniformLocationMVP = glGetUniformLocation(program, "MVP");

	return (Shader){.program = program, .locationMVP = uniformLocationMVP};
}
void Shader_Destroy(const Shader shader)
{
	glDeleteProgram(shader.program);
}
void Shader_SetMVP(const Shader shader, const mat4 mvp)
{
	mat4 matMVP = {
		{mvp[0][0], mvp[0][1], mvp[0][2], mvp[0][3]},
		{mvp[1][0], mvp[1][1], mvp[1][2], mvp[1][3]},
		{mvp[2][0], mvp[2][1], mvp[2][2], mvp[2][3]},
		{mvp[3][0], mvp[3][1], mvp[3][2], mvp[3][3]},
	};
	glUniformMatrix4fv(shader.locationMVP, 1, GL_FALSE, (GLfloat *)matMVP);
}
void Shader_Apply(const Shader shader)
{
	glUseProgram(shader.program);
}
void Shader_GetUniformLocations(const Shader shader, UniformLocation *uniformLocations, int uniformCount)
{
	for (int i = 0; i < uniformCount; i++) {
		uniformLocations[i].location = glGetUniformLocation(shader.program, uniformLocations[i].name);
	}
}