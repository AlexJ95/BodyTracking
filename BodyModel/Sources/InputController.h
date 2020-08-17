#pragma once

#include <Kore/Input/Keyboard.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Graphics4/PipelineState.h>
#include <Kore/Math/Quaternion.h>
#include <map>

#include "CustomMath.h"

using namespace Kore;

class InputController
{
	vec4 camUp;
	vec4 camForward;
	vec4 camRight;

	vec3 cameraPos;
	// Keyboard controls
	bool rotate = false;
	bool W = false, A = false, S = false, D = false;

	std::map<KeyCode, void()> *callbacks;

	CustomMath* math;

public:
	InputController(std::map<KeyCode, void()> callbacks[]);

	void keyDown(KeyCode code); //manages some keyboard keys

	void keyUp(KeyCode code); //similar to keyDown, but as the name implies

	void mouseMove(int windowId, int x, int y, int movementX, int movementY); //mousecontrols for the camera

	void mousePress(int windowId, int button, int x, int y); //just sets the bool rotate to true, doesn't use its parameters strangely

	void mouseRelease(int windowId, int button, int x, int y); //just like mousepress but sets rotate to false

	void update(float deltaT);
	void init();
};

