#pragma once

#include "Settings.h"
#include "pch.h"

#include <Kore/Math/Quaternion.h>

class CustomMath
{
private:
	static CustomMath* instance; //This class is designed to be a singleton
public:
	Kore::vec3 cameraPos;
	Kore::vec4 camUp;
	Kore::vec4 camForward;
	Kore::vec4 camRight;

	Kore::mat4 initTrans;
	Kore::mat4 initTransInv;
	Kore::Quaternion initRot;
	Kore::Quaternion initRotInv;
	
	static CustomMath* getInstance(); // Get the singleton instance

	CustomMath();

	Kore::mat4 getMirrorMatrix(); // Esed to mirror avatar in demo

	Kore::mat4 getProjectionMatrix(); // Not sure what exactly this one does

	Kore::mat4 getViewMatrix(); // Get the matrix describing the players view

	void setSetViewMatrixCoefficients(Kore::vec3 cameraPosition, Kore::vec4);

	void rotateCamera(int movementX, int movementY);

	void initTransAndRot(); // Initializes the quaternion initRot and matrix initTrans and their inverses

	void init();
};