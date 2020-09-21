#include "InputController.h"

InputController* InputController::instance;

InputController* InputController::getInstance()
{
	if (!instance) instance = new InputController();
	return instance;
}

InputController* InputController::getInstanceAndAppend(std::map<Kore::KeyCode, void(*)()> callbackMap)
{
	if (!instance) instance = new InputController(callbackMap);
	else instance->callbacks.insert(callbackMap.begin(), callbackMap.end());
	return instance;
}

InputController::InputController()
{
	math = math->getInstance();
	init();
}

InputController::InputController(std::map<Kore::KeyCode, void(*)()> callbackMap) : InputController()
{
	callbacks = callbackMap;
}

void InputController::setUI(UI3D* uiReference)
{
	ui = uiReference;
}

void InputController::setAnimatorAndAvatar(Animator* animatorReference, Avatar* avatarReference)
{
	animator = animatorReference;
	avatar = avatarReference;
}

void keyDown(Kore::KeyCode code)
{
	InputController* inputController;
	inputController->getInstance()->keyDown(code);
}

void InputController::keyDown(Kore::KeyCode code)
{
	if (callbacks.find(code) != callbacks.end()) callbacks.at(code)();

	switch (code) {
	case Kore::KeyW:
		W = true;
		break;
	case Kore::KeyA:
		A = true;
		break;
	case Kore::KeyS:
		S = true;
		break;
	case Kore::KeyD:
		D = true;
		break;
	case Kore::KeyT:
		T = true;
		break;
	case Kore::KeyR:
#ifdef KORE_STEAMVR
		Kore::VrInterface::resetHmdPose();
#endif
		break;
	default:
		break;
	}
}

void keyUp(Kore::KeyCode code)
{
	InputController* inputController;
	inputController->getInstance()->keyUp(code);
}

void InputController::keyUp(Kore::KeyCode code)
{
	switch (code) {
	case Kore::KeyW:
		W = false;
		break;
	case Kore::KeyA:
		A = false;
		break;
	case Kore::KeyS:
		S = false;
		break;
	case Kore::KeyD:
		D = false;
		break;
	case Kore::KeyT:
		T = false;
		break;
	default:
		break;
	}
}

void mouseMove(int windowId, int x, int y, int movementX, int movementY)
{
	InputController* inputController;
	inputController->getInstance()->mouseMove(windowId, x, y, movementX, movementY);
}

void InputController::mouseMove(int windowId, int x, int y, int movementX, int movementY)
{
	if (rotate & !ui->lockBackground()) math->rotateCamera(movementX, movementY);
}

void mousePress(int windowId, int button, int x, int y)
{
	InputController* inputController;
	inputController->getInstance()->mousePress(windowId, button, x, y);
}

void InputController::mousePress(int windowId, int button, int x, int y)
{
	if (!ui->isUIshown())
		rotate = true;
}

void mouseRelease(int windowId, int button, int x, int y)
{
	InputController* inputController;
	inputController->getInstance()->mouseRelease(windowId, button, x, y);
}

void InputController::mouseRelease(int windowId, int button, int x, int y)
{
	rotate = false;
}

#ifdef KORE_STEAMVR
void gamepadButton(int buttonNr, float value)
{
	InputController* inputController;
	inputController->getInstance()->gamepadButton(buttonNr, value);
}

void InputController::gamepadButton(int buttonNr, float value)
{
	// Grip button => set size and reset an avatar to a default T-Pose
	if (buttonNr == 2 && value == 1) animator->resetAvatarPose(avatar);

	// Menu button => calibrate
	if (buttonNr == 1 && value == 1) animator->calibrateAvatar(avatar);
}
#endif

void initBindings()
{
	//register Mouse and Keyboard
	Kore::Keyboard::the()->KeyDown = keyDown;
	Kore::Keyboard::the()->KeyUp = keyUp;
	Kore::Mouse::the()->Move = mouseMove;
	Kore::Mouse::the()->Press = mousePress;
	Kore::Mouse::the()->Release = mouseRelease;

#ifdef KORE_STEAMVR
	//register GamePad Buttons
	VrPoseState controller;
	int count = 0;
	for (int i = 0; i < 16; ++i) {
		controller = Kore::VrInterface::getController(i);
		if (controller.trackedDevice == TrackedDevice::Controller) {
			Kore::Gamepad::get(i)->Button = gamepadButton;
			++count;
		}
	}
	assert(count == 2);
#endif
}

void InputController::init()
{
	if (initialized) return;
	initBindings();
	math->init();
	initialized = true;
}

void InputController::update(float deltaT)
{
	if (!ui->isUIshown()) math->moveCamera(W, A, S, D, deltaT);

	if(!ui->isUIshown() && (avatar->lastMovement == 9)) math->moveCamera(true, false, false, false, deltaT);
	
}