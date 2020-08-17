#pragma once

#include <Kore/Math/Quaternion.h>

#include "Settings.h"
#include "Animator.h"
#include "CustomMath.h"
#include "AnimatedEntity.h"

using namespace Kore;

class Calibrator
{
#ifdef KORE_STEAMVR
	bool controllerButtonsInitialized = false;
	float currentUserHeight;
	bool firstPersonMonitor = false;
#endif

	bool calibratedAvatar = false;

	CustomMath* math;

public:
	Calibrator();

	void calibrate(Avatar* avatar, Animator* animator); //initially calibrates the avatar

	void setSize(); //adjust avatar size to player

	void initEndEffector(int efID, int deviceID, vec3 pos, Kore::Quaternion rot);

	void assignControllerAndTracker();

	void gamepadButton(int buttonNr, float value); //Confusing name, seems to control initial calibration

	void initButtons();
};