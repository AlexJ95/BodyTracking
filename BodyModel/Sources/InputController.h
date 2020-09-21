#pragma once

#include "CustomMath.h"
#include "Animator.h"

#ifdef KORE_STEAMVR
#include <Kore/Vr/VrInterface.h>
#include <Kore/Input/Gamepad.h>
#endif

#include <Kore/Input/Keyboard.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Math/Quaternion.h>
#include "UI3D.h"

#include <map>

class InputController
{
	static InputController* instance;
	bool initialized = false;

	std::map<Kore::KeyCode, void(*)()> callbacks;

	UI3D* ui;
	CustomMath* math;
	Animator* animator;
	Avatar* avatar;

	// Keyboard and Mouse controls
	bool rotate = false;
	bool W = false, A = false, S = false, D = false, T = false;

public:
	static InputController* getInstance(); // Get the singleton instance
	static InputController* getInstanceAndAppend(std::map<Kore::KeyCode, void(*)()> callbackArray); // Get the singleton instance
	
	InputController();
	InputController(std::map<Kore::KeyCode, void(*)()> callbacks);

	void setUI(UI3D* uiReference);
	void setAnimatorAndAvatar(Animator* animatorReference, Avatar* avatarReference);

	void keyDown(Kore::KeyCode code); //manages some keyboard keys
	void keyUp(Kore::KeyCode code); //similar to keyDown, but as the name implies
	void mouseMove(int windowId, int x, int y, int movementX, int movementY); //mousecontrols for the camera
	void mousePress(int windowId, int button, int x, int y); //just sets the bool rotate to true, doesn't use its parameters strangely
	void mouseRelease(int windowId, int button, int x, int y); //just like mousepress but sets rotate to false
	void gamepadButton(int buttonNr, float value); //manages VR Controller inputs

	void init();
	
	void update(float deltaT);
};
