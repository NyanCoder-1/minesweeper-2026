#include "app-context.h"
#include "game/player-controller.h"
#include "cglm/util.h"
#include "game/camera-controller.h"

PlayerController PlayerController_Create(AppContext *appContext, vec3 initialPosition, vec3 initialRotation)
{
	PlayerController playerController = {.appContext = appContext, .position = {initialPosition[0], initialPosition[1], initialPosition[2]}, .rotation = {initialRotation[0], initialRotation[1], initialRotation[2]}};

	return playerController;
}
void PlayerController_Destroy(PlayerController *playerController)
{
	if (!playerController) {
		return;
	}
}
void PlayerController_Update(PlayerController *self, float deltaTime)
{
	if (!self) {
		return;
	}
	self->rotation[0] = glm_clamp(self ->rotation[0] + self->appContext->input.lookY * M_PI / 720.0f, -M_PI_2, M_PI_2);
	self->rotation[1] += self->appContext->input.lookX * M_PI / 720.0f;
	self->position[0] += (sinf(self->rotation[1]) * self->appContext->input.moveForward + sinf(self->rotation[1] + M_PI_2) * self->appContext->input.moveSide) * deltaTime * self->appContext->playerSpeed;
	self->position[2] += (cosf(self->rotation[1]) * self->appContext->input.moveForward + cosf(self->rotation[1] + M_PI_2) * self->appContext->input.moveSide) * deltaTime * self->appContext->playerSpeed;

	CameraController_SetPosition(&self->appContext->camera, (vec3){self->position[0], self->position[1] + 1.6f, self->position[2]});
	CameraController_SetRotation(&self->appContext->camera, (vec3){self->rotation[0], self->rotation[1], self->rotation[2]});
}
void PlayerController_Render(const PlayerController self)
{
}