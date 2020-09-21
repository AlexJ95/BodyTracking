#include "Animator.h"

Animator::Animator(Avatar* avatar) {
	math = CustomMath::getInstance();
	motionRecognizer = new MachineLearningMotionRecognition(avatar);
}

bool Animator::executeAnimation(AnimatedEntity* entity, const char* filename, Logger* logger)
{
	float scaleFactor;
	Kore::vec3 desPosition[numOfEndEffectors];
	Kore::Quaternion desRotation[numOfEndEffectors];
	bool inAnimation = logger->readData(numOfEndEffectors, filename, desPosition, desRotation, scaleFactor);

	for (int i = 0; i < numOfEndEffectors; ++i) 
	{
		entity->endEffector[i]->setDesPosition(desPosition[i]);
		entity->endEffector[i]->setDesRotation(desRotation[i]);
	}

	
	if (!entity->calibrated) 
	{
		resetPositionAndRotation(entity);
		entity->meshObject->setScale(scaleFactor);
		BoneNode* bones[numOfEndEffectors];
		for (int i = 0; i < numOfEndEffectors; i++) bones[i] = getBoneWithIndex(entity, entity->endEffector[i]->getBoneIndex());
		calibrate(entity, bones);
		entity->calibrated = true;
	}
	
	for (int i = 0; i < numOfEndEffectors; ++i) executeMovement(entity, i);

	entity->calibrated = inAnimation;
	return inAnimation;
}

void Animator::rigVrPose(Avatar* avatar)
{
#ifdef KORE_STEAMVR
	Kore::VrInterface::begin();

	VrPoseState vrDevice;
	for (int i = 0; i < numOfEndEffectors; ++i) {
		if (avatar->endEffector[i]->getDeviceIndex() != -1) {

			if (i == head) {
				SensorState state = Kore::VrInterface::getSensorState(0);

				// Get HMD position and rotation
				avatar->endEffector[i]->setDesPosition(state.pose.vrPose.position);
				avatar->endEffector[i]->setDesRotation(state.pose.vrPose.orientation);
			}
			else {
				vrDevice = Kore::VrInterface::getController(avatar->endEffector[i]->getDeviceIndex());

				// Get VR device position and rotation
				avatar->endEffector[i]->setDesPosition(vrDevice.vrPose.position);
				avatar->endEffector[i]->setDesRotation(vrDevice.vrPose.orientation);
			}

			executeMovement(avatar, i);
		}
	}
#endif
}

void Animator::executeMovement(AnimatedEntity* entity, int endEffectorID)
{
	Kore::Quaternion locRot(0, 0, 0, 1);
	locRot.rotate(entity->rotation);
	locRot.normalize();
	Kore::Quaternion locRotInv = locRot.invert();

	Kore::vec3 locPos = Kore::vec4(0, 0, 0, 1);
	Kore::mat4 locTrans = Kore::mat4::Translation(locPos.x(), locPos.y(), locPos.z()) * locRot.matrix().Transpose();
	Kore::mat4 locTransInv = locTrans.Invert();


	Kore::vec3 desPosition = entity->endEffector[endEffectorID]->getDesPosition();
	Kore::Quaternion desRotation = entity->endEffector[endEffectorID]->getDesRotation();

	// Save raw data
	//if (logRawData && typeid(entity) == typeid(Avatar)) logger->saveData(entity->endEffector[endEffectorID]->getName(), desPosition, desRotation, entity->scale);

	if (entity->calibrated) {
		// Transform desired position/rotation to the character local coordinate system
		if (isnan(locRotInv.x))
		{
			desPosition = CustomMath::getInstance()->initTransInv * Kore::vec4(desPosition.x(), desPosition.y(), desPosition.z(), 1);
			desRotation = CustomMath::getInstance()->initRotInv.rotated(desRotation);
		}
		else
		{
			desPosition = locTransInv * CustomMath::getInstance()->initTransInv * Kore::vec4(desPosition.x(), desPosition.y(), desPosition.z(), 1);
			desRotation = locRotInv.rotated(CustomMath::getInstance()->initRotInv.rotated(desRotation));
		}


		// Add offset
		Kore::Quaternion offsetRotation = entity->endEffector[endEffectorID]->getOffsetRotation();
		Kore::vec3 offsetPosition = entity->endEffector[endEffectorID]->getOffsetPosition();
		Kore::Quaternion finalRot = desRotation.rotated(offsetRotation);
		Kore::vec3 finalPos = Kore::mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z()) * finalRot.matrix().Transpose() * Kore::mat4::Translation(offsetPosition.x(), offsetPosition.y(), offsetPosition.z()) * Kore::vec4(0, 0, 0, 1);
		finalPos += entity->position;

		if (endEffectorID == hip) {
			setFixedPositionAndOrientation(entity, entity->endEffector[endEffectorID]->getBoneIndex(), finalPos, finalRot);
		}
		else if (endEffectorID == head || endEffectorID == leftForeArm || endEffectorID == rightForeArm || endEffectorID == leftFoot || endEffectorID == rightFoot) {
			setDesiredPositionAndOrientation(entity, entity->endEffector[endEffectorID]->getBoneIndex(), entity->endEffector[endEffectorID]->getIKMode(), finalPos, finalRot);
		}
		else if (endEffectorID == leftHand || endEffectorID == rightHand) {
			setFixedOrientation(entity, entity->endEffector[endEffectorID]->getBoneIndex(), finalRot);
		}

		if (motionRecognizer->isProcessingMovementData() && !static_cast<Avatar*>(entity))
		{
			// if we are not using actual VR sensors, we cannot retrieve the velocity values and have to use defaults
						// if we do use VR sensors, the actual velocity can be used
			Kore::vec3 rawAngVel;
			Kore::Quaternion desAngVel;
			Kore::vec3 rawLinVel;
			Kore::vec3 desLinVel;
			Kore::vec3 rawPosition;
			Kore::Quaternion rawRotation;

			// actual VR hardware present
#ifdef KORE_STEAMVR

			VrPoseState sensorState;
			// retrieve sensor or HMD state (data is the same, retrieval is slightly different)
			if (endEffectorID == head) {
				sensorState = VrInterface::getSensorState(0).pose;
			}
			else {
				sensorState = VrInterface::getController(endEffector[endEffectorID]->getDeviceIndex());
			}

			// collect data
			rawLinVel = sensorState.linearVelocity;
			rawAngVel = sensorState.angularVelocity;
			desLinVel = initTransInv * vec4(rawLinVel.x(), rawLinVel.y(), rawLinVel.z(), 1);
			Kore::Quaternion rawAngVelQuat = toQuaternion(rawAngVel);
			desAngVel = initRotInv.rotated(rawAngVelQuat);
			rawPosition = sensorState.vrPose.position;
			rawRotation = sensorState.vrPose.orientation;

			// no actual VR hardware present
#else
	// these placeholder values are only used for testing with predetermined movement sets, not for recording new data
			rawAngVel = Kore::vec3(1, 2, 3);
			desAngVel = desRotation;
			rawLinVel = Kore::vec3(7, 8, 9);
			desLinVel = rawLinVel;
			rawPosition = desPosition;
			rawRotation = desRotation;
#endif

			// forward data
			motionRecognizer->processMovementData(
				entity->endEffector[endEffectorID]->getName(),
				rawPosition, desPosition, finalPos,
				rawRotation, desRotation, finalRot,
				rawAngVel, desAngVel,
				rawLinVel, desLinVel,
				entity->meshObject->scale, lastTime);
		}
	}
}


void Animator::setDesiredPositionAndOrientation(AnimatedEntity* entity, int boneIndex, IKMode ikMode, Kore::vec3 desPosition, Kore::Quaternion desRotation) {
	BoneNode* bone = getBoneWithIndex(entity, boneIndex);

	entity->invKin->inverseKinematics(bone, ikMode, desPosition, desRotation);
}

void Animator::setFixedPositionAndOrientation(AnimatedEntity* entity, int boneIndex, Kore::vec3 desPosition, Kore::Quaternion desRotation) {
	BoneNode* bone = getBoneWithIndex(entity, boneIndex);

	bone->transform = Kore::mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z());
	bone->rotation = desRotation;
	bone->rotation.normalize();
	bone->local = bone->transform * bone->rotation.matrix().Transpose();
}

void Animator::setFixedOrientation(AnimatedEntity* entity, int boneIndex, Kore::Quaternion desRotation) {
	BoneNode* bone = getBoneWithIndex(entity, boneIndex);

	Kore::Quaternion localRot;
	Kore::RotationUtility::getOrientation(&bone->parent->combined, &localRot);
	bone->rotation = localRot.invert().rotated(desRotation);

	//bone->transform = mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z());
	//bone->rotation = desRotation;

	bone->rotation.normalize();
	bone->local = bone->transform * bone->rotation.matrix().Transpose();
}

BoneNode* Animator::getBoneWithIndex(AnimatedEntity* entity, int boneIndex) const {
	BoneNode* bone = entity->meshObject->bones[boneIndex - 1];
	return bone;
}

void Animator::resetPositionAndRotation(AnimatedEntity* entity) {
	for (int i = 0; i < entity->meshObject->bones.size(); ++i) {
		entity->meshObject->bones[i]->transform = entity->meshObject->bones[i]->bind;
		entity->meshObject->bones[i]->local = entity->meshObject->bones[i]->bind;
		entity->meshObject->bones[i]->combined = entity->meshObject->bones[i]->parent->combined * entity->meshObject->bones[i]->local;
		entity->meshObject->bones[i]->combinedInv = entity->meshObject->bones[i]->combined.Invert();
		entity->meshObject->bones[i]->finalTransform = entity->meshObject->bones[i]->combined * entity->meshObject->bones[i]->combinedInv;
		entity->meshObject->bones[i]->rotation = Kore::Quaternion(0, 0, 0, 1);
	}
}

void Animator::calibrate(AnimatedEntity* entity, BoneNode* bones[numOfEndEffectors])
{
	CustomMath::getInstance()->initTransAndRot();

	for (int i = 0; i < numOfEndEffectors; ++i) {
		Kore::vec3 desPosition = entity->endEffector[i]->getDesPosition();
		Kore::Quaternion desRotation = entity->endEffector[i]->getDesRotation();

		// Transform desired position/rotation to the character local coordinate system
		desPosition = CustomMath::getInstance()->initTransInv * Kore::vec4(desPosition.x(), desPosition.y(), desPosition.z(), 1);
		desRotation = CustomMath::getInstance()->initRotInv.rotated(desRotation);

		// Get actual position/rotation of the character skeleton
		//BoneNode* bone = animator->getBoneWithIndex(avatar, avatar->endEffector[i]->getBoneIndex());
		Kore::vec3 targetPos = bones[i]->getPosition();
		Kore::Quaternion targetRot = bones[i]->getOrientation();

		entity->endEffector[i]->setOffsetPosition((Kore::mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z()) * targetRot.matrix().Transpose()).Invert() * Kore::mat4::Translation(targetPos.x(), targetPos.y(), targetPos.z()) * Kore::vec4(0, 0, 0, 1));
		entity->endEffector[i]->setOffsetRotation((desRotation.invert()).rotated(targetRot));
	}
}

#ifdef KORE_STEAMVR
void Animator::setSize(Avatar* avatar)
{
	float currentAvatarHeight = getCurrentHeight(avatar);

	SensorState state = Kore::VrInterface::getSensorState(0);
	Kore::vec3 hmdPos = state.pose.vrPose.position; // z -> face, y -> up down
	float currentUserHeight = hmdPos.y();

	float scale = currentUserHeight / currentAvatarHeight;
	avatar->meshObject->setScale(scale);
}

void Animator::initEndEffector(Avatar* avatar, int efID, int deviceID, Kore::vec3 pos, Kore::Quaternion rot)
{
	avatar->endEffector[efID]->setDeviceIndex(deviceID);
	avatar->endEffector[efID]->setDesPosition(pos);
	avatar->endEffector[efID]->setDesRotation(rot);
}

void Animator::assignControllerAndTracker(Avatar* avatar)
{
	VrPoseState vrDevice;

	const int numTrackers = 5;
	int trackerCount = 0;

	std::vector<EndEffector*> trackers;

	// Get indices for VR devices
	for (int i = 0; i < 16; ++i) {
		vrDevice = Kore::VrInterface::getController(i);

		Kore::vec3 devicePos = vrDevice.vrPose.position;
		Kore::Quaternion deviceRot = vrDevice.vrPose.orientation;

		if (vrDevice.trackedDevice == TrackedDevice::ViveTracker) {
			EndEffector* tracker = new EndEffector(-1);
			tracker->setDeviceIndex(i);
			tracker->setDesPosition(devicePos);
			tracker->setDesRotation(deviceRot);
			trackers.push_back(tracker);

			++trackerCount;
			if (trackerCount == numTrackers) {
				// Sort trackers regarding the y-Axis (height)
				std::sort(trackers.begin(), trackers.end(), sortByYAxis());

				// Left or Right Leg
				std::sort(trackers.begin(), trackers.begin() + 2, sortByZAxis());
				initEndEffector(avatar, leftFoot, trackers[0]->getDeviceIndex(), trackers[0]->getDesPosition(), trackers[0]->getDesRotation());
				initEndEffector(avatar, rightFoot, trackers[1]->getDeviceIndex(), trackers[1]->getDesPosition(), trackers[1]->getDesRotation());

				// Hip
				initEndEffector(avatar, hip, trackers[2]->getDeviceIndex(), trackers[2]->getDesPosition(), trackers[2]->getDesRotation());

				// Left or Right Forearm
				std::sort(trackers.begin() + 3, trackers.begin() + 5, sortByZAxis());
				initEndEffector(avatar, leftForeArm, trackers[3]->getDeviceIndex(), trackers[3]->getDesPosition(), trackers[3]->getDesRotation());
				initEndEffector(avatar, rightForeArm, trackers[4]->getDeviceIndex(), trackers[4]->getDesPosition(), trackers[4]->getDesRotation());
			}


		}
		else if (vrDevice.trackedDevice == TrackedDevice::Controller) {
			// Hand controller
			if (devicePos.z() > 0) {
				initEndEffector(avatar, rightHand, i, devicePos, deviceRot);
			}
			else {
				initEndEffector(avatar, leftHand, i, devicePos, deviceRot);
			}
		}
	}

	// HMD
	SensorState stateLeftEye = Kore::VrInterface::getSensorState(0);
	SensorState stateRightEye = Kore::VrInterface::getSensorState(1);
	Kore::vec3 leftEyePos = stateLeftEye.pose.vrPose.position;
	Kore::vec3 rightEyePos = stateRightEye.pose.vrPose.position;
	Kore::vec3 hmdPosCenter = (leftEyePos + rightEyePos) / 2;
	initEndEffector(avatar, head, 0, hmdPosCenter, stateLeftEye.pose.vrPose.orientation);
}

void Animator::resetAvatarPose(Avatar* avatar)
{
	avatar->calibrated = false;
	CustomMath::getInstance()->initTransAndRot();
	resetPositionAndRotation(avatar);
	setSize(avatar);
}

void Animator::calibrateAvatar(Avatar* avatar)
{
	assignControllerAndTracker(avatar);
	BoneNode* bones[numOfEndEffectors];
	for (int i = 0; i < numOfEndEffectors; i++) bones[i] = getBoneWithIndex(avatar, avatar->endEffector[i]->getBoneIndex());
	calibrate(avatar, bones);
	avatar->calibrated = true;
	//log(Info, "Calibrate avatar");
}

float Animator::getCurrentHeight(AnimatedEntity* entity) const {
	// Get the highest position
	BoneNode* head = getBoneWithIndex(entity, headBoneIndex);
	Kore::vec4 position = head->combined * Kore::vec4(0, 0, 0, 1);
	position *= 1.0 / position.w();
	return position.z();
}
#endif