#include "InputController.h"

InputController::InputController(std::map<Kore::KeyCode, void ()> callbackArray[]) {
	camUp = Kore::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	camForward = Kore::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	camRight = Kore::vec4(1.0f, 0.0f, 0.0f, 0.0f);

	cameraPos = Kore::vec3(0, 0, 0);

	callbacks = callbackArray;
	math = math->getInstance();
}

void InputController::keyDown(Kore::KeyCode code)
{
	//if (callbacks->find(code) != callbacks->end()) callbacks->at(code)();

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
	Kore::Quaternion q2(camRight, 0.01f * -movementY);

	camUp = q2.matrix() * camUp;
	camRight = q1.matrix() * camRight;

	q2.rotate(q1);
	Kore::mat4 mat = q2.matrix();
	camForward = mat * camForward;
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
	if (S) cameraPos -= camForward * (float)deltaT * cameraMoveSpeed;
	if (W) cameraPos += camForward * (float)deltaT * cameraMoveSpeed;
	if (A) cameraPos += camRight * (float)deltaT * cameraMoveSpeed;
	if (D) cameraPos -= camRight * (float)deltaT * cameraMoveSpeed;
}

void InputController::init() {
	math->initTransAndRot();

	// Set camera initial position and orientation
	cameraPos = Kore::vec3(2.6, 1.8, 0.0);
	Kore::Quaternion q1(Kore::vec3(0.0f, 1.0f, 0.0f), Kore::pi / 2.0f);
	Kore::Quaternion q2(Kore::vec3(1.0f, 0.0f, 0.0f), -Kore::pi / 8.0f);
	camUp = q2.matrix() * camUp;
	camRight = q1.matrix() * camRight;
	q2.rotate(q1);
	Kore::mat4 mat = q2.matrix();
	camForward = mat * camForward;

	//TODO: fix this pls
	/*
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	Mouse::the()->Move = mouseMove;
	Mouse::the()->Press = mousePress;
	Mouse::the()->Release = mouseRelease;
	*/
}
