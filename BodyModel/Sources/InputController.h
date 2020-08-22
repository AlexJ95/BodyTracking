#pragma once

#include "CustomMath.h"

#include <Kore/Input/Keyboard.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Math/Quaternion.h>

#include <map>

class InputController
{
	static InputController* instance;

	CustomMath* math;

	std::map<Kore::KeyCode, void(*)()> callbacks;

	bool initialized = false;

	// Keyboard and Mouse controls
	bool rotate = false;
	bool W = false, A = false, S = false, D = false;
	float cameraMoveSpeed = 4.f;
	
public:
	static InputController* getInstance(); // Get the singleton instance
	static InputController* getInstanceAndAppend(std::map<Kore::KeyCode, void(*)()> callbacks); // Get the singleton instance and add new key-controls
	
	InputController();
	InputController(std::map<Kore::KeyCode, void(*)()> callbacks);

	void keyDown(Kore::KeyCode code); //manages some keyboard keys

	void keyUp(Kore::KeyCode code); //similar to keyDown, but as the name implies

	void mouseMove(int windowId, int x, int y, int movementX, int movementY); //mousecontrols for the camera

	void mousePress(int windowId, int button, int x, int y); //just sets the bool rotate to true, doesn't use its parameters strangely

	void mouseRelease(int windowId, int button, int x, int y); //just like mousepress but sets rotate to false

	void init();

	void update(float deltaT);
};
