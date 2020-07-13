
#include <Kore/Math/Quaternion.h>

#include "Settings.h"
#include "Animator.h"
#include "CustomMath.h"

using namespace Kore;

class Calibrator
{
#ifdef KORE_STEAMVR
	bool controllerButtonsInitialized = false;
	float currentUserHeight;
	bool firstPersonMonitor = false;
#else
	int loop = 0;
#endif
	bool calibratedAvatar = false;

	CustomMath* math;

	Animator* animator;
public:
	Calibrator(Animator* animator);

	void calibrate(); //initially calibrates the avatar

	void setSize(); //adjust avatar size to player

	void initEndEffector(int efID, int deviceID, vec3 pos, Kore::Quaternion rot);

	void assignControllerAndTracker();

	void gamepadButton(int buttonNr, float value); //Confusing name, seems to control initial calibration

	void initButtons();
};