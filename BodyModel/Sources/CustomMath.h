#pragma once

#include <Kore/Math/Quaternion.h>

#include "Settings.h"
#include "pch.h"

class CustomMath
{
	static CustomMath* instance; //This class is designed to be a singleton
public:
	Kore::mat4 initTrans;
	Kore::mat4 initTransInv;
	Kore::Quaternion initRot;
	Kore::Quaternion initRotInv;
	Kore::vec3 cameraPos;
	Kore::vec4 camForward;
	
	static CustomMath* getInstance(); // Get the singleton instance

	Kore::mat4 getMirrorMatrix(); // Esed to mirror avatar in demo

	Kore::mat4 getProjectionMatrix(); // Not sure what exactly this one does

	Kore::mat4 getViewMatrix(); // Get the matrix describing the players view

	void setSetViewMatrixCoefficients(Kore::vec3 cameraPosition, Kore::vec4);

	void initTransAndRot(); // Initializes the quaternion initRot and matrix initTrans and their inverses
};