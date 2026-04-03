#include "game/input-controller.h"
#include "SDL3/SDL_events.h"
#include <math.h>
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
	}
}
void InputController_Update(InputController *self)
{
	if (!self) {
		return;
	}

	float deltaForward = 0.0f;
	float deltaRight = 0.0f;

	// Keyboard input
	deltaForward += (self->isButtonW ? 1.0f : 0.0f) + (self->isButtonS ? -1.0f : 0.0f);
	deltaRight += (self->isButtonA ? -1.0f : 0.0) + (self->isButtonD ? 1.0f : 0.0);
	// Normalize diagonal movement
	if (deltaForward * deltaForward + deltaRight * deltaRight > 1.0f) {
		float speedDiagonal = sinf(M_PI_2);
		deltaForward *= speedDiagonal;
		deltaRight *= speedDiagonal;
	}
	self->moveSide = deltaRight;
	self->moveForward = deltaForward;
	self->lookX = self->mouseDeltaX;
	self->lookY = self->mouseDeltaY;

	self->isLBMPrev = self->isLBM;
	self->isRMBPrev = self->isRMB;
	self->isGamepadButtonSouthPrev = self->isGamepadButtonSouth;
	self->isGamepadButtonEastPrev = self->isGamepadButtonEast;
	self->isTouchscreenButtonDigPrev = self->isTouchscreenButtonDig;
	self->isTouchscreenButtonFlagPrev = self->isTouchscreenButtonFlag;
}
