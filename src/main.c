#include "app-context-pre.h"
#include "app-context.h"
#include "cglm/cam.h"
#include "cglm/cglm.h" // IWYU pragma: export
#include "game/block.h"
#include "game/camera-controller.h"
#include "game/input-controller.h"
#include "game/player-controller.h"
#include "render/builtin-shaders.h"
#include "render/mesh.h"   // IWYU pragma: keep
#include "render/shader.h" // IWYU pragma: keep
#include "render/type-vertex.h"
#include <GLES3/gl3.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#if __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

static void main_loop(AppContext *ctx);

static void preUpdate(AppContext *ctx);
static void events(AppContext *ctx);
static void windowResize(AppContext *ctx);
static double deltaTime(AppContext *ctx);
static void update(AppContext *ctx, double deltaTime);
static void render(AppContext *ctx);

static int createWindow(AppContext *ctx);
static int initGL(AppContext *ctx);
static void initGame(AppContext *ctx);
static void freeResources(AppContext *ctx);

SDL_AppResult SDL_Fail()
{
	SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
	return SDL_APP_FAILURE;
}
int main(void)
{
	AppContext ctx;
	memset(&ctx, 0, sizeof(AppContext));

	int result = createWindow(&ctx);
	if (result) {
		return result;
	}
	result = initGL(&ctx);
	if (result) {
		return result;
	}
	initGame(&ctx);

	clock_gettime(CLOCK_MONOTONIC, &ctx.timePrev);
#if __EMSCRIPTEN__
	// on Emscripten, we cannot have an infinite loop in main. Instead, we must
	// tell emscripten to call our main loop.
	emscripten_set_main_loop_arg((void (*)(void *))main_loop, &ctx, 0, 1);
#else
	while (ctx.isRunning) {
		main_loop(&ctx);
	}
#endif

	freeResources(&ctx);
	return 0;
}

static void main_loop(AppContext *ctx)
{
	preUpdate(ctx);
	events(ctx);
	windowResize(ctx);
	double dTime = deltaTime(ctx);
	update(ctx, dTime);
	render(ctx);
}

static void preUpdate(AppContext *ctx)
{
	InputController_PreUpdate(&ctx->input);
}
static void events(AppContext *ctx)
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT) {
			ctx->isRunning = false;
		}
		if (event.type == SDL_EVENT_MOUSE_MOTION) {
			ctx->mousePositionX = event.motion.x;
			ctx->mousePositionY = event.motion.y;
		}
		InputController_Event(&ctx->input, &event);
	}
}
static void windowResize(AppContext *ctx)
{
	int width, height;
	SDL_GetWindowSizeInPixels(ctx->window, &width, &height);
	bool isResized = ctx->windowWidth != width || ctx->windowHeight != height;
	if (isResized) {
		ctx->windowWidth = width;
		ctx->windowHeight = height;
		glViewport(0, 0, ctx->windowWidth, ctx->windowHeight);
		glm_ortho(0.f, ctx->windowWidth, ctx->windowHeight, 0.f, 1.f, -1.f, ctx->matrixViewProjection);
		ctx->isResize = true;
	} else {
		ctx->isResize = false;
	}
}
static double deltaTime(AppContext *ctx)
{
	struct timespec timeCurrent;
	clock_gettime(CLOCK_MONOTONIC, &timeCurrent); // Never use `gettimeofday()` for dTime, you could use that for current time, like for clock, but not for dTime
	double deltaTime = (timeCurrent.tv_sec - ctx->timePrev.tv_sec) + (timeCurrent.tv_nsec - ctx->timePrev.tv_nsec) / 1000000000.0;
	ctx->timePrev = timeCurrent;
	return deltaTime;
}
static void update(AppContext *ctx, double deltaTime)
{
	mat4 matProjection;
	glm_perspective(glm_rad(ctx->screenFOV), ctx->windowWidth / (float)(ctx->windowHeight), 0.1f, 100.f, matProjection);
	mat4 matView;
	CameraController_GetViewMatrix(ctx->camera, matView);
	glm_mat4_mul(matProjection, matView, ctx->matrixViewProjection);
	InputController_Update(&ctx->input);
	PlayerController_Update(&ctx->player, deltaTime);
	CameraController_Update(&ctx->camera, deltaTime);
}
static void render(AppContext *ctx)
{
	glClearColor(0x07 / 255.f, 0x07 / 255.f, 0x07 / 255.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Shader_Apply(ctx->shaderSolid);
	// Shader_SetMVP(ctx->shaderSolid, ctx->matrixViewProjection);
	// Mesh_Render(ctx->mesh);
	PlayerController_Render(ctx->player);
	Block_Render(ctx->block);

	SDL_GL_SwapWindow(ctx->window);

}

enum WindowInitialSize {
#ifndef __EMSCRIPTEN__
	WindowInitialWidth = 1280,
	WindowInitialHeight = 720,
#endif
	WindowMinimumWidth = 640,
	WindowMinimumHeight = 360,
};
static int createWindow(AppContext *ctx)
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
		return SDL_Fail();
	}
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
#ifdef __EMSCRIPTEN__
	int WindowInitialWidth = 1280;
	int WindowInitialHeight = 720;
	emscripten_get_canvas_element_size("#canvas", &WindowInitialWidth, &WindowInitialHeight);
#endif
	ctx->window = SDL_CreateWindow("Window", WindowInitialWidth, WindowInitialHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!ctx->window) {
		return SDL_Fail();
	}
	SDL_SetWindowMinimumSize(ctx->window, WindowMinimumWidth, WindowMinimumHeight);
	SDL_SetWindowRelativeMouseMode(ctx->window, true);
	return 0;
}
static int initGL(AppContext *ctx)
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GLContext glContext = SDL_GL_CreateContext(ctx->window);
	if (!glContext) {
		return SDL_Fail();
	}
	SDL_ShowWindow(ctx->window);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	SDL_GetWindowSize(ctx->window, &ctx->windowWidth, &ctx->windowHeight);
	glViewport(0, 0, ctx->windowWidth, ctx->windowHeight);
	glm_ortho(0.f, ctx->windowWidth, ctx->windowHeight, 0.f, 1.f, -1.f, ctx->matrixViewProjection);

	return 0;
}
static void initGame(AppContext *ctx)
{
	ctx->shaderSolid = Shader_Create(shaderVertexSolidSource, shaderFragmentSolidSource, vertexSolidAttributeLocations, vertexSolidAttributeLocationsLength);
	ctx->shaderSolidColored = Shader_Create(shaderVertexSolidSource, shaderFragmentColoredSource, vertexSolidAttributeLocations, vertexSolidAttributeLocationsLength);
	ctx->shaderTextured = Shader_Create(shaderVertexTexturedSource, shaderFragmentTexturedSource, vertexTexturedAttributeLocations, vertexTexturedAttributeLocationsLength);
	ctx->shaderQuadratic = Shader_Create(shaderVertexQuadraticSource, shaderFragmentQuadraticSource, vertexQuadraticAttributeLocations, vertexQuadraticAttributeLocationsLength);
	ctx->shaderQuadraticSolidColored = Shader_Create(shaderVertexQuadraticSource, shaderFragmentColoredSource, vertexQuadraticAttributeLocations, vertexQuadraticAttributeLocationsLength);
	ctx->shaderQuadraticColored = Shader_Create(shaderVertexQuadraticSource, shaderFragmentQuadraticColoredSource, vertexQuadraticAttributeLocations, vertexQuadraticAttributeLocationsLength);
	ctx->shaderMSDF = Shader_Create(shaderVertexTexturedSource, shaderFragmentMSDFSource, vertexTexturedAttributeLocations, vertexTexturedColoredAttributeLocationsLength);
	ctx->shaderMSDFColored = Shader_Create(shaderVertexTexturedColoredSource, shaderFragmentMSDFColoredSource, vertexTexturedColoredAttributeLocations, vertexTexturedColoredAttributeLocationsLength);
	ctx->isRunning = true;
	ctx->input = InputController_Create();

	ctx->camera = CameraController_Create(ctx, (vec3){2.0f, 1.6f, -4.0f}, (vec3){0.0f, 0.0f, 0.0f});
	struct TriangleIndicesU8 {
		uint8_t indices[3];
	};
	ctx->mesh = Mesh_Create(vertexSolidDescription, vertexSolidDescriptionLength,
							(VertexSolid[]){
								(VertexSolid){.position = {1.0f, 1.0f, 1.0f}, .color = {1.0f, 1.0f, 1.0f, 1.0f}},
								(VertexSolid){.position = {1.0f, 0.0f, 1.0f}, .color = {1.0f, 0.0f, 1.0f, 1.0f}},
								(VertexSolid){.position = {1.0f, 1.0f, -1.0f}, .color = {1.0f, 1.0f, 0.0f, 1.0f}},
								(VertexSolid){.position = {1.0f, 0.0f, -1.0f}, .color = {1.0f, 0.0f, 0.0f, 1.0f}},
								(VertexSolid){.position = {-1.0f, 1.0f, -1.0f}, .color = {0.0f, 1.0f, 0.0f, 1.0f}},
								(VertexSolid){.position = {-1.0f, 0.0f, -1.0f}, .color = {0.0f, 0.0f, 0.0f, 1.0f}},
								(VertexSolid){.position = {-1.0f, 1.0f, 1.0f}, .color = {0.0f, 1.0f, 1.0f, 1.0f}},
								(VertexSolid){.position = {-1.0f, 0.0f, 1.0f}, .color = {0.0f, 0.0f, 1.0f, 1.0f}},
							},
							sizeof(VertexSolid) * 8,
							(struct TriangleIndicesU8[]){
								{0, 1, 2}, {1, 3, 2},
								{2, 3, 4}, {3, 5, 4},
								{4, 5, 6}, {5, 7, 6},
								{6, 7, 0}, {7, 1, 0},
								{0, 2, 4}, {0, 4, 6},
							},
							sizeof(struct TriangleIndicesU8) * 10, MeshIndexTypeUint8);

	ctx->screenFOV = 75.0f;
	ctx->playerSpeed = 3.0f;

	ctx->player = PlayerController_Create(ctx, (vec3){0.0f, 0.0f, 0.0f}, (vec3){0.0f, 0.0f, 0.0f});

	ctx->block = Block_Create(ctx, BLOCK_1, (vec3){0.0f, 0.0f, 0.0f}, (vec2){1.0f, 1.0f}, Block_Shadows(true, false, true, false));
}
static void freeResources(AppContext *ctx)
{
	Block_Destroy(&ctx->block);

	Mesh_Destroy(&ctx->mesh);

	Shader_Destroy(ctx->shaderSolid);
	Shader_Destroy(ctx->shaderSolidColored);
	Shader_Destroy(ctx->shaderTextured);
	Shader_Destroy(ctx->shaderQuadratic);
	Shader_Destroy(ctx->shaderQuadraticSolidColored);
	Shader_Destroy(ctx->shaderQuadraticColored);

	PlayerController_Destroy(&ctx->player);

#if !__EMSCRIPTEN__
	SDL_DestroyWindow(ctx->window);
	SDL_Quit();
#endif
}
