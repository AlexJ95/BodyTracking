
#include "CustomMath.h"

// Variables to mirror the room and the avatar
vec3 mirrorOver(6.057f, 0.0f, 0.04f);



CustomMath* CustomMath::getInstance()
{
	if (!instance)
		instance = new CustomMath;
	return instance;
}

mat4 CustomMath::getMirrorMatrix() {
	Kore::Quaternion rot(0, 0, 0, 1);
	rot.rotate(Kore::Quaternion(vec3(0, 1, 0), Kore::pi));
	mat4 zMirror = mat4::Identity();
	zMirror.Set(2, 2, -1);
	Kore::mat4 M = zMirror * mat4::Translation(mirrorOver.x(), mirrorOver.y(), mirrorOver.z()) * rot.matrix().Transpose();

	return M;
}

Kore::mat4 CustomMath::getProjectionMatrix() {
	mat4 P = mat4::Perspective(45, (float)width / (float)height, 0.01f, 1000);
	P.Set(0, 0, -P.get(0, 0));

	return P;
}

Kore::mat4 CustomMath::getViewMatrix() {
	mat4 V = mat4::lookAlong(camForward.xyz(), cameraPos, vec3(0.0f, 1.0f, 0.0f));
	return V;
}

void CustomMath::setSetViewMatrixCoefficients(vec3 cameraPosition, vec4 camForwardRef)
{
	cameraPos = cameraPosition;
	camForward = camForwardRef;
}

void CustomMath::initTransAndRot() {
	initRot = Kore::Quaternion(0, 0, 0, 1);
	initRot.rotate(Kore::Quaternion(vec3(1, 0, 0), -Kore::pi / 2.0));
	initRot.rotate(Kore::Quaternion(vec3(0, 0, 1), Kore::pi / 2.0));
	initRot.normalize();
	initRotInv = initRot.invert();

	vec3 initPos = vec4(0, 0, 0, 1);
	initTrans = mat4::Translation(initPos.x(), initPos.y(), initPos.z()) * initRot.matrix().Transpose();
	initTransInv = initTrans.Invert();
}
