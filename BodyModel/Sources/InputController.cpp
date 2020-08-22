#include "InputController.h"

InputController* InputController::instance;

InputController* InputController::getInstance()
{
	if (!instance)
		instance = new InputController();
	return instance;
}

InputController* InputController::getInstanceAndAppend(std::map<Kore::KeyCode, void(*)()> callbackMap)
{
	if (!instance)
		instance = new InputController(callbackMap);
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
	case Kore::KeyR:
#ifdef KORE_STEAMVR
		VrInterface::resetHmdPose();
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
	if (rotate) math->rotateCamera(movementX, movementY);
}

void mousePress(int windowId, int button, int x, int y)
{
	InputController* inputController;
	inputController->getInstance()->mousePress(windowId, button, x, y);
}

void InputController::mousePress(int windowId, int button, int x, int y)
{
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

void initBindings()
{
	Kore::Keyboard::the()->KeyDown = keyDown;
	Kore::Keyboard::the()->KeyUp = keyUp;
	Kore::Mouse::the()->Move = mouseMove;
	Kore::Mouse::the()->Press = mousePress;
	Kore::Mouse::the()->Release = mouseRelease;
}

void InputController::init()
{
	if (!initialized) return;
	initBindings();
	math->initTransAndRot();
}

void InputController::update(float deltaT)
{
	// Move position of camera based on WASD keys
	Kore::vec3 cameraPos = math->cameraPos;
	if (S) cameraPos -= math->camForward * (float)deltaT * cameraMoveSpeed;
	if (W) cameraPos += math->camForward * (float)deltaT * cameraMoveSpeed;
	if (A) cameraPos += math->camRight * (float)deltaT * cameraMoveSpeed;
	if (D) cameraPos -= math->camRight * (float)deltaT * cameraMoveSpeed;
	math->cameraPos = cameraPos;
}