#include "InputController.h"

InputController* InputController::instance;

InputController* InputController::getInstanceAndAppend()
{
	if (!instance)
		instance = new InputController();
	return instance;
}

InputController* InputController::getInstanceAndAppend(std::map<Kore::KeyCode, void(*)()> callbackArray)
{
	if (!instance)
		instance = new InputController(callbackArray);
	else instance->callbacks.insert(callbackArray.begin(), callbackArray.end());
	return instance;
}

InputController::InputController() { math = math->getInstance(); }

InputController::InputController(std::map<Kore::KeyCode, void(*)()> callbackArray) : InputController() {
	callbacks = callbackArray;
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
		/*
	case KeyL:
		//Kore::log(Kore::LogLevel::Info, "cameraPos: (%f, %f, %f)", cameraPos.x(), cameraPos.y(), cameraPos.z());
		//Kore::log(Kore::LogLevel::Info, "camUp: (%f, %f, %f, %f)", camUp.x(), camUp.y(), camUp.z(), camUp.w());
		//Kore::log(Kore::LogLevel::Info, "camRight: (%f, %f, %f, %f)", camRight.x(), camRight.y(), camRight.z(), camRight.w());
		//Kore::log(Kore::LogLevel::Info, "camForward: (%f, %f, %f, %f)", camForward.x(), camForward.y(), camForward.z(), camForward.w());

		record();
		break;
	case Kore::KeyEscape:
	case KeyQ:
		System::stop();
		break;*/
	default:
		break;
	}
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

void InputController::mouseMove(int windowId, int x, int y, int movementX, int movementY)
{
	Kore::Quaternion q1(Kore::vec3(0.0f, 1.0f, 0.0f), 0.01f * movementX);
	Kore::Quaternion q2(math->camRight, 0.01f * -movementY);

	math->camUp = q2.matrix() * math->camUp;
	math->camRight = q1.matrix() * math->camRight;

	q2.rotate(q1);
	Kore::mat4 mat = q2.matrix();
	math->camForward = mat * math->camForward;
}

void InputController::mousePress(int windowId, int button, int x, int y)
{
	rotate = true;
}

void InputController::mouseRelease(int windowId, int button, int x, int y)
{
	rotate = false;
}

void InputController::update(float deltaT)
{
	// Move position of camera based on WASD keys
	float cameraMoveSpeed = 4.f;
	Kore::vec3 cameraPos = math->cameraPos;
	if (S) cameraPos -= math->camForward * (float)deltaT * cameraMoveSpeed;
	if (W) cameraPos += math->camForward * (float)deltaT * cameraMoveSpeed;
	if (A) cameraPos += math->camRight * (float)deltaT * cameraMoveSpeed;
	if (D) cameraPos -= math->camRight * (float)deltaT * cameraMoveSpeed;
	math->cameraPos = cameraPos;
}

void InputController::init() {
	math->camUp = Kore::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	math->camForward = Kore::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	math->camRight = Kore::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	math->cameraPos = Kore::vec3(0, 0, 0);
	math->initTransAndRot();
}