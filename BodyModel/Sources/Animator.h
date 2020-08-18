#pragma once

#include <Kore/Math/Quaternion.h>
#include <typeinfo>

#include "Settings.h"
#include "CustomMath.h"
#include "AnimatedEntity.h"
//#include "Logger.h"

class Animator
{
	CustomMath* math;

	//Logger* logger;

public :
	Animator();

	void executeMovement(AnimatedEntity* entity, int endEffectorID); //moves a singular endeffector to desired attitude
	
	void setDesiredPositionAndOrientation(AnimatedEntity* entity, int boneIndex, IKMode ikMode, Kore::vec3 desPosition, Kore::Quaternion desRotation);

	void setFixedPositionAndOrientation(AnimatedEntity* entity, int boneIndex, Kore::vec3 desPosition, Kore::Quaternion desRotation);

	void setFixedOrientation(AnimatedEntity* entity,int boneIndex, Kore::Quaternion desRotation);

	BoneNode* getBoneWithIndex(AnimatedEntity* entity, int index) const;

	void resetPositionAndRotation(AnimatedEntity* entity);

	float getReached(AnimatedEntity* entity) const;
	float getStucked(AnimatedEntity* entity) const;
	float* getIterations(AnimatedEntity* entity) const;
	float* getErrorPos(AnimatedEntity* entity) const;
	float* getErrorRot(AnimatedEntity* entity) const;
	float* getTime(AnimatedEntity* entity) const;
	float* getTimeIteration(AnimatedEntity* entity) const;
	float getCurrentHeight(AnimatedEntity* entity) const;
};