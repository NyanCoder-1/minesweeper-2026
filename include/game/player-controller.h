#ifndef __GAME_PLAYER_CONTROLLER_H__
#define __GAME_PLAYER_CONTROLLER_H__

typedef struct PlayerController PlayerController;
#include "app-context-pre.h"
#include "cglm/cglm.h" // IWYU pragma: export
struct PlayerController {
	AppContext *appContext;

	vec3 position;
	vec3 rotation;
};
PlayerController PlayerController_Create(AppContext *appContext, vec3 initialPosition, vec3 initialRotation);
void PlayerController_Destroy(PlayerController *playerController);
void PlayerController_Update(PlayerController *self, float deltaTime);
void PlayerController_Render(const PlayerController self);


#endif
