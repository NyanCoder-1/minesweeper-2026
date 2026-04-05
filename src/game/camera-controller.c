#include "game/camera-controller.h"
#include "cglm/mat4.h"
#include <math.h>

CameraController CameraController_Create(AppContext *appContext, vec3 initialPosition, vec3 initialRotation)
{
	return (CameraController){.appContext = appContext, .position = {initialPosition[0], initialPosition[1], initialPosition[2]}, .rotation = {initialRotation[0], initialRotation[1], initialRotation[2]}};
}
void CameraController_SetPosition(CameraController *self, vec3 newPosition)
{
	self->position[0] = newPosition[0];
	self->position[1] = newPosition[1];
	self->position[2] = newPosition[2];
}
void CameraController_SetRotation(CameraController *self, vec3 newRotation)
{
	self->rotation[0] = newRotation[0];
	self->rotation[1] = newRotation[1];
	self->rotation[2] = newRotation[2];
}
void CameraController_GetPosition(const CameraController self, vec3 vecOut)
{
	if (!vecOut) {
		return;
	}
	vecOut[0] = self.position[0];
	vecOut[1] = self.position[1];
	vecOut[2] = self.position[2];
}
void CameraController_GetRotation(const CameraController self, vec3 vecOut)
{
	if (!vecOut) {
		return;
	}
	vecOut[0] = self.rotation[0];
	vecOut[1] = self.rotation[1];
	vecOut[2] = self.rotation[2];
}
void CameraController_GetViewMatrix(const CameraController self, mat4 matOut)
{
	if (!matOut) {
		return;
	}
	const vec3 lookat = {self.position[0], self.position[1], self.position[2] + 1.0f};
	glm_lookat((vec3){self.position[0], self.position[1], self.position[2]}, (vec3){lookat[0], lookat[1], lookat[2]}, (vec3){0.0, 1.0, 0.0}, matOut);
	mat4 matRotation;
	glm_rotate_x(GLM_MAT4_IDENTITY, -self.rotation[0], matRotation);
	glm_rotate_y(matRotation, -self.rotation[1], matRotation);
	glm_rotate_z(matRotation, -self.rotation[2], matRotation);
	glm_mat4_mul(matRotation, matOut, matOut);
}
void CameraController_Update(CameraController *self, double deltaTime)
{
	if (!self) {
		return;
	}
}
