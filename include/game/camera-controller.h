#ifndef __GAME_CAMERA_CONTROLLER_H__
#define __GAME_CAMERA_CONTROLLER_H__

#include "app-context-pre.h"
#include "cglm/cglm.h" // IWYU pragma: export
typedef struct CameraController CameraController;

struct CameraController {
	AppContext *appContext;

	vec3 position;
	vec3 rotation;
};

CameraController CameraController_Create(AppContext *appContext, vec3 initialPosition, vec3 initialRotation);
void CameraController_SetPosition(CameraController *self, vec3 newPosition);
void CameraController_SetRotation(CameraController *self, vec3 newRotation);
void CameraController_GetPosition(const CameraController self, vec3 vecOut);
void CameraController_GetRotation(const CameraController self, vec3 vecOut);
void CameraController_GetViewMatrix(const CameraController self, mat4 matOut);
void CameraController_Update(CameraController *self, double deltaTime);

#endif
