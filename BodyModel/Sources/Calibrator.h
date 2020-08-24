#pragma once

#include "Settings.h"
//#include "Animator.h"
#include "CustomMath.h"
#include "AnimatedEntity.h"

#include <Kore/Math/Quaternion.h>

class Calibrator
{
#ifdef KORE_STEAMVR
	bool controllerButtonsInitialized = false;
	float currentUserHeight;
#endif

	CustomMath* math;

public:
	Calibrator();

	void calibrate(AnimatedEntity* entity, BoneNode* bone[numOfEndEffectors]); //initially calibrates the avatar

	void setSize(); //adjust avatar size to player

	void initEndEffector(int efID, int deviceID, Kore::vec3 pos, Kore::Quaternion rot);

	void assignControllerAndTracker();

	void gamepadButton(int buttonNr, float value); //Confusing name, seems to control initial calibration

	void initButtons();
};