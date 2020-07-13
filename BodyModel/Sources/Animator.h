#pragma once

#include <Kore/Math/Quaternion.h>
#include <typeinfo>

#include "Settings.h"
#include "CustomMath.h"
#include "Logger.h"
#include "Renderer.h"

using namespace Kore;

class Animator
{
	CustomMath* math;

	Logger* logger;

public :
	EndEffector** endEffector;
	InverseKinematics* invKin;
	Renderer::HumanoidEntity* entity;
	float currentHeight;

	Animator();
	Animator(Renderer::HumanoidEntity* humanoidEntity);

	void executeMovement(int endEffectorID); //moves a singular endeffector to desired attitude
	
	void setDesiredPositionAndOrientation(int boneIndex, IKMode ikMode, Kore::vec3 desPosition, Kore::Quaternion desRotation);

	void setFixedPositionAndOrientation(int boneIndex, Kore::vec3 desPosition, Kore::Quaternion desRotation);

	void setFixedOrientation(int boneIndex, Kore::Quaternion desRotation);

	BoneNode* getBoneWithIndex(int index) const;

	void resetPositionAndRotation();

	float getReached() const;
	float getStucked() const;
	float* getIterations() const;
	float* getErrorPos() const;
	float* getErrorRot() const;
	float* getTime() const;
	float* getTimeIteration() const;
	float getHeight() const;
};