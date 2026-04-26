#ifndef __APP_CONTEXT_H__
#define __APP_CONTEXT_H__

#include "app-context-pre.h" // IWYU pragma: export
#include "cglm/cglm.h"		 // IWYU pragma: export
#include "game/block.h"
#include "game/camera-controller.h"
#include "game/input-controller.h"
#include "game/player-controller.h"
#include "render/mesh.h"
#include "render/shader.h"
#include <GLES3/gl3.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>

#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 199309L
#define _POSIX_C_SOURCE 199309L
#endif
#include <time.h>

struct AppContext {
	SDL_Window *window;

	// Window info
	int windowWidth;
	int windowHeight;
	double mousePositionX;
	double mousePositionY;

	// Rendering stuff
	Shader shaderSolid;
	Shader shaderSolidColored;
	Shader shaderTextured;
	Shader shaderQuadratic;
	Shader shaderQuadraticSolidColored;
	Shader shaderQuadraticColored;
	Shader shaderMSDF;
	Shader shaderMSDFColored;
	mat4 matrixViewProjection;

	struct timespec timePrev;

	InputController input;

	// Objects
	Mesh mesh;
	CameraController camera;
	float angle;
	PlayerController player;
	Block block;

	float playerSpeed;
	float screenFOV;

	// bit-fields
	bool isRunning : 1;
	bool isResize : 1;
};

#endif
