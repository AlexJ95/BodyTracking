#pragma once

#include <Kore/Input/Keyboard.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Graphics4/PipelineState.h>
#include <Kore/Math/Quaternion.h>
#include <map>

#include "CustomMath.h"

class InputController
{
	Kore::vec4 camUp;
	Kore::vec4 camForward;
	Kore::vec4 camRight;

	Kore::vec3 cameraPos;
	// Keyboard controls
	bool rotate = false;
	bool W = false, A = false, S = false, D = false;

	std::map<Kore::KeyCode, void()> *callbacks;

	CustomMath* math;

public:
	InputController(std::map<Kore::KeyCode, void()> callbacks[]);

	void keyDown(Kore::KeyCode code); //manages some keyboard keys

	void keyUp(Kore::KeyCode code); //similar to keyDown, but as the name implies

	void mouseMove(int windowId, int x, int y, int movementX, int movementY); //mousecontrols for the camera

	void mousePress(int windowId, int button, int x, int y); //just sets the bool rotate to true, doesn't use its parameters strangely

	void mouseRelease(int windowId, int button, int x, int y); //just like mousepress but sets rotate to false

	void update(float deltaT);
	void init();
};

