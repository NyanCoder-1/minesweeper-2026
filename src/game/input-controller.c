#include "game/input-controller.h"
#include "SDL3/SDL_events.h"
#include <math.h>
#include <stdint.h>
#include <string.h>

InputController InputController_Create()
{
	InputController result;
	memset(&result, 0, sizeof(result));
	return result;
}
void InputController_PreUpdate(InputController *self)
{
	if (!self) {
		return;
	}
	self->isDig = false;
	self->isFlag = false;
	self->isCheck = false;
	self->mouseDeltaX = 0.0f;
	self->mouseDeltaY = 0.0f;
	if (SDL_HasGamepad()) {
		if (self->gamepad && !SDL_GamepadConnected(self->gamepad)) {
			SDL_CloseGamepad(self->gamepad);
			self->gamepad = NULL;
		}
		if (!self->gamepad) {
			int count = 0;
			SDL_JoystickID *ids = SDL_GetGamepads(&count);
			// Iterate over the list of gamepads
			for(int i = 0; i < count; i++) {
				SDL_Gamepad* gamepd = SDL_OpenGamepad(ids[i]);
				if(self->gamepad == NULL) {
					self->gamepad = gamepd;
				} else {
					SDL_CloseGamepad(gamepd);
					continue;
				}
				// Close the other gamepads
				if(i > 0) {
					SDL_CloseGamepad(gamepd);
				}
			}
		}
	}
}
void InputController_Event(InputController *self, const SDL_Event *event)
{
	if (!self) {
		return;
	}
	if (event->type == SDL_EVENT_KEY_DOWN) {
		if (event->key.scancode == SDL_SCANCODE_W) {
			self->isButtonW = true;
		} else if (event->key.scancode == SDL_SCANCODE_A) {
			self->isButtonA = true;
		} else if (event->key.scancode == SDL_SCANCODE_S) {
			self->isButtonS = true;
		} else if (event->key.scancode == SDL_SCANCODE_D) {
			self->isButtonD = true;
		}
	} else if (event->type == SDL_EVENT_KEY_UP) {
		if (event->key.scancode == SDL_SCANCODE_W) {
			self->isButtonW = false;
		} else if (event->key.scancode == SDL_SCANCODE_A) {
			self->isButtonA = false;
		} else if (event->key.scancode == SDL_SCANCODE_S) {
			self->isButtonS = false;
		} else if (event->key.scancode == SDL_SCANCODE_D) {
			self->isButtonD = false;
		}
	} else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event->button.button == SDL_BUTTON_LEFT) {
			self->isLBM = true;
		} else if (event->button.button == SDL_BUTTON_RIGHT) {
			self->isRMB = true;
		}
	} else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
		if (event->button.button == SDL_BUTTON_LEFT) {
			self->isLBM = false;
		} else if (event->button.button == SDL_BUTTON_RIGHT) {
			self->isRMB = false;
		}
	} else if (event->type == SDL_EVENT_MOUSE_MOTION) {
		self->mouseDeltaX = event->motion.xrel;
		self->mouseDeltaY = event->motion.yrel;
	} else if (event->type == SDL_EVENT_GAMEPAD_AXIS_MOTION) {
		if (event->gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX) {
			self->gamepadLeftStickX = event->gaxis.value / (float)(INT16_MAX + 1);
		} else if (event->gaxis.axis == SDL_GAMEPAD_AXIS_LEFTY) {
			self->gamepadLeftStickY = event->gaxis.value / (float)(INT16_MAX + 1);
		} else if (event->gaxis.axis == SDL_GAMEPAD_AXIS_RIGHTX) {
			self->gamepadRightStickX = event->gaxis.value / (float)(INT16_MAX + 1);
		} else if (event->gaxis.axis == SDL_GAMEPAD_AXIS_RIGHTY) {
			self->gamepadRightStickY = event->gaxis.value / (float)(INT16_MAX + 1);
		}
	}
}
void InputController_Update(InputController *self)
{
	if (!self) {
		return;
	}

	float deltaForward = 0.0f;
	float deltaRight = 0.0f;

	static const float GAMEPAD_DEADZONE = 0.1f;
	// Keyboard input
	deltaForward += (self->isButtonW ? 1.0f : 0.0f) + (self->isButtonS ? -1.0f : 0.0f) - (fabsf(self->gamepadLeftStickY) > GAMEPAD_DEADZONE ? self->gamepadLeftStickY : 0.0f);
	deltaRight += (self->isButtonA ? -1.0f : 0.0) + (self->isButtonD ? 1.0f : 0.0) + (fabsf(self->gamepadLeftStickX) > GAMEPAD_DEADZONE ? self->gamepadLeftStickX : 0.0f);
	// Normalize diagonal movement
	const float sqrMovement = deltaForward * deltaForward + deltaRight * deltaRight;
	if (sqrMovement > 1.0f) {
		const float speedDiagonal = sqrt(sqrMovement);
		deltaForward /= speedDiagonal;
		deltaRight /= speedDiagonal;
	}
	self->moveSide = deltaRight;
	self->moveForward = deltaForward;
	self->lookX = self->mouseDeltaX;
	self->lookY = self->mouseDeltaY;
	self->lookXSmooth = fabsf(self->gamepadRightStickX) > GAMEPAD_DEADZONE ? self->gamepadRightStickX : 0.0f;
	self->lookYSmooth = fabsf(self->gamepadRightStickY) > GAMEPAD_DEADZONE ? self->gamepadRightStickY : 0.0f;

	self->isLBMPrev = self->isLBM;
	self->isRMBPrev = self->isRMB;
	self->isGamepadButtonSouthPrev = self->isGamepadButtonSouth;
	self->isGamepadButtonEastPrev = self->isGamepadButtonEast;
	self->isTouchscreenButtonDigPrev = self->isTouchscreenButtonDig;
	self->isTouchscreenButtonFlagPrev = self->isTouchscreenButtonFlag;
}
