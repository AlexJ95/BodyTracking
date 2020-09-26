#pragma once

#include "Settings.h"
#include "CustomMath.h"
#include "AnimatedEntity.h"
#include "Logger.h"
#include "MachineLearningMotionRecognition.h"

#ifdef KORE_STEAMVR
#include <Kore/Vr/VrInterface.h>
#include <Kore/Vr/SensorState.h>
#endif

#include <Kore/Math/Quaternion.h>

#include <algorithm>
#include <typeinfo>

class Animator
{
#ifdef KORE_STEAMVR
	float currentUserHeight;
#endif


	CustomMath* math;
	MachineLearningMotionRecognition* motionRecognizer;
	
	AnimatedEntity* animatedentity;
	const char* animatedfilename; 
	Logger* animatedlogger;

public:
	bool inAnimation = false;

	Animator();

	void update(double deltaT);

	bool executeAnimation(AnimatedEntity* entity, const char* filename, Logger* logger);

	void rigVrPose(Avatar* avatar);

	void executeMovement(AnimatedEntity* entity, int endEffectorID); //moves a singular endeffector to desired attitude

	void feedMovementRecognition();
	
	void setDesiredPositionAndOrientation(AnimatedEntity* entity, int boneIndex, IKMode ikMode, Kore::vec3 desPosition, Kore::Quaternion desRotation);

	void setFixedPositionAndOrientation(AnimatedEntity* entity, int boneIndex, Kore::vec3 desPosition, Kore::Quaternion desRotation);

	void setFixedOrientation(AnimatedEntity* entity,int boneIndex, Kore::Quaternion desRotation);

	BoneNode* getBoneWithIndex(AnimatedEntity* entity, int index) const;

	void resetPositionAndRotation(AnimatedEntity* entity);

	void calibrate(AnimatedEntity* entity, BoneNode* bone[numOfEndEffectors]); //initially calibrates the avatar

	void setSize(Avatar* avatar); //adjust avatar size to player

	void initEndEffector(Avatar* avatar, int efID, int deviceID, Kore::vec3 pos, Kore::Quaternion rot);

	void assignControllerAndTracker(Avatar* avatar);

	void resetAvatarPose(Avatar* avatar);

	void calibrateAvatar(Avatar* avatar);

	float getCurrentHeight(AnimatedEntity* entity) const;
};