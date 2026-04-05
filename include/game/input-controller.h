#ifndef __GAME_INPUT_CONTROLLER_H__
#define __GAME_INPUT_CONTROLLER_H__

#include "SDL3/SDL_events.h"
#include <stdbool.h>
typedef struct InputController InputController;
enum InputMode {
	InputMode_KeyboardMouse,
	InputMode_Gamepad,
	InputMode_Touchscreen,
};
struct InputController {
	SDL_Gamepad* gamepad;

	//  Presision inputs
	// ! Actual game inputs
	float moveSide;
	float moveForward;
	float lookX;
	float lookY;
	// - Mouse
	float mouseDeltaX;
	float mouseDeltaY;
	// - Gamepad
	float gamepadLeftStickX;
	float gamepadLeftStickY;
	float gamepadRightStickX;
	float gamepadRightStickY;
	// - Touchscreen
	float touchscreenWalkStickX;
	float touchscreenWalkStickY;
	float touchscreenLookStickX;
	float touchscreenLookStickY;

	// Buttons
	// ! Actual game inputs
	bool isDig : 1;
	bool isFlag : 1;
	bool isCheck : 1;
	// - Mouse+Keyboard
	bool isButtonW : 1;
	bool isButtonA : 1;
	bool isButtonS : 1;
	bool isButtonD : 1;
	bool isLBM : 1;
	bool isLBMPrev : 1; //< for detecting start of click
	bool isRMB : 1;
	bool isRMBPrev : 1; //< for detecting start of click
	// - Gamepad
	bool isGamepadButtonSouth : 1;
	bool isGamepadButtonSouthPrev : 1; //< for detecting start of press
	bool isGamepadButtonEast : 1;
	bool isGamepadButtonEastPrev : 1; //< for detecting start of press
	// - Touchscreen
	bool isTouchscreenButtonDig : 1;
	bool isTouchscreenButtonDigPrev : 1; //< for detecting start of press
	bool isTouchscreenButtonFlag : 1;
	bool isTouchscreenButtonFlagPrev : 1; //< for detecting start of press
};
InputController InputController_Create();
void InputController_PreUpdate(InputController *self);
void InputController_Event(InputController *self, const SDL_Event *event);
void InputController_Update(InputController *self);

#endif
