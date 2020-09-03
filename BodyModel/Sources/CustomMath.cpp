
#include "CustomMath.h"

// Variables to mirror the room and the avatar
Kore::vec3 mirrorOver(6.057f, 0.0f, 0.04f);

CustomMath* CustomMath::instance;

CustomMath* CustomMath::getInstance()
{
	if (!instance)
		instance = new CustomMath();
	return instance;
}

CustomMath::CustomMath()
{
	camUp = Kore::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	camForward = Kore::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	camRight = Kore::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	cameraPos = Kore::vec3(0, 0, 0);
}

Kore::mat4 CustomMath::getMirrorMatrix()
{
	Kore::Quaternion rot(0, 0, 0, 1);
	rot.rotate(Kore::Quaternion(Kore::vec3(0, 1, 0), Kore::pi));
	Kore::mat4 zMirror = Kore::mat4::Identity();
	zMirror.Set(2, 2, -1);
	Kore::mat4 M = zMirror * Kore::mat4::Translation(mirrorOver.x(), mirrorOver.y(), mirrorOver.z()) * rot.matrix().Transpose();

	return M;
}

void CustomMath::setProjectionAndViewMatrices(Kore::mat4 projectionMatrix, Kore::mat4 viewMatrix)
{
	vrProjectionMatrix = projectionMatrix;
	vrViewMatrix = viewMatrix;
}

Kore::mat4 CustomMath::getProjectionMatrix()
{
#ifdef KORE_STEAMVR
	if (hmdMode || !firstPersonMonitor) return vrProjectionMatrix;
#endif
	Kore::mat4 P = Kore::mat4::Perspective(45, (float)width / (float)height, 0.01f, 1000);
	P.Set(0, 0, -P.get(0, 0));
	return P;
}

Kore::mat4 CustomMath::getViewMatrix()
{
#ifdef KORE_STEAMVR
	if (hmdMode || !firstPersonMonitor) return vrViewMatrix;
#endif
	Kore::mat4 V = Kore::mat4::lookAlong(camForward.xyz(), cameraPos, Kore::vec3(0.0f, 1.0f, 0.0f));
	return V;
}

void CustomMath::setSetViewMatrixCoefficients(Kore::vec3 cameraPosition, Kore::vec4 camForwardRef)
{
	cameraPos = cameraPosition;
	camForward = camForwardRef;
}

void CustomMath::rotateCamera(int movementX, int movementY)
{
	Kore::Quaternion q1(Kore::vec3(0.0f, 1.0f, 0.0f), 0.01f * movementX);
	Kore::Quaternion q2(camRight, 0.01f * -movementY);

	camUp = q2.matrix() * camUp;
	camRight = q1.matrix() * camRight;

	q2.rotate(q1);
	Kore::mat4 mat = q2.matrix();
	camForward = mat * camForward;
}

void CustomMath::initTransAndRot() {
	initRot = Kore::Quaternion(0, 0, 0, 1);
	initRot.rotate(Kore::Quaternion(Kore::vec3(1, 0, 0), -Kore::pi / 2.0));
	initRot.rotate(Kore::Quaternion(Kore::vec3(0, 0, 1), Kore::pi / 2.0));
	initRot.normalize();
	initRotInv = initRot.invert();

	Kore::vec3 initPos = Kore::vec4(0, 0, 0, 1);
	initTrans = Kore::mat4::Translation(initPos.x(), initPos.y(), initPos.z()) * initRot.matrix().Transpose();
	initTransInv = initTrans.Invert();
}

void CustomMath::init()
{
	// Set camera initial position and orientation
	cameraPos = Kore::vec3(2.6, 1.8, 0.0);
	Kore::Quaternion q1(Kore::vec3(0.0f, 1.0f, 0.0f), Kore::pi / 2.0f);
	Kore::Quaternion q2(Kore::vec3(1.0f, 0.0f, 0.0f), -Kore::pi / 8.0f);
	camUp = q2.matrix() * camUp;
	camRight = q1.matrix() * camRight;
	q2.rotate(q1);
	Kore::mat4 mat = q2.matrix();
	camForward = mat * camForward;

	initTransAndRot();
}
