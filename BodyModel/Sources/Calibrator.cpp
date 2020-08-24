
#include "Calibrator.h"

Calibrator::Calibrator() {
	math = math->getInstance();
}

void Calibrator::calibrate(AnimatedEntity* entity, BoneNode* bones[numOfEndEffectors]) {
	math->initTransAndRot();
	
	for (int i = 0; i < numOfEndEffectors; ++i) {
		Kore::vec3 desPosition = entity->endEffector[i]->getDesPosition();
		Kore::Quaternion desRotation = entity->endEffector[i]->getDesRotation();

		// Transform desired position/rotation to the character local coordinate system
		desPosition = math->initTransInv * Kore::vec4(desPosition.x(), desPosition.y(), desPosition.z(), 1);
		desRotation = math->initRotInv.rotated(desRotation);

		// Get actual position/rotation of the character skeleton
		//BoneNode* bone = animator->getBoneWithIndex(avatar, avatar->endEffector[i]->getBoneIndex());
		Kore::vec3 targetPos = bones[i]->getPosition();
		Kore::Quaternion targetRot = bones[i]->getOrientation();

		entity->endEffector[i]->setOffsetPosition((Kore::mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z()) * targetRot.matrix().Transpose()).Invert() * Kore::mat4::Translation(targetPos.x(), targetPos.y(), targetPos.z()) * Kore::vec4(0, 0, 0, 1));
		entity->endEffector[i]->setOffsetRotation((desRotation.invert()).rotated(targetRot));
	}
}

#ifdef KORE_STEAMVR
void Calibrator::setSize() {
	float currentAvatarHeight = avatar->getHeight();

	SensorState state = VrInterface::getSensorState(0);
	vec3 hmdPos = state.pose.vrPose.position; // z -> face, y -> up down
	currentUserHeight = hmdPos.y();

	float scale = currentUserHeight / currentAvatarHeight;
	avatar->setScale(scale);

	log(Info, "current avatar height %f, current user height %f ==> scale %f", currentAvatarHeight, currentUserHeight, scale);
}

void Calibrator::initEndEffector(int efID, int deviceID, vec3 pos, Kore::Quaternion rot) {
	endEffector[efID]->setDeviceIndex(deviceID);
	endEffector[efID]->setDesPosition(pos);
	endEffector[efID]->setDesRotation(rot);

	log(Info, "%s, device id: %i", endEffector[efID]->getName(), deviceID);
}

void Calibrator::assignControllerAndTracker() {
	VrPoseState vrDevice;

	const int numTrackers = 5;
	int trackerCount = 0;

	std::vector<EndEffector*> trackers;

	// Get indices for VR devices
	for (int i = 0; i < 16; ++i) {
		vrDevice = VrInterface::getController(i);

		vec3 devicePos = vrDevice.vrPose.position;
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
				initEndEffector(leftFoot, trackers[0]->getDeviceIndex(), trackers[0]->getDesPosition(), trackers[0]->getDesRotation());
				initEndEffector(rightFoot, trackers[1]->getDeviceIndex(), trackers[1]->getDesPosition(), trackers[1]->getDesRotation());

				// Hip
				initEndEffector(hip, trackers[2]->getDeviceIndex(), trackers[2]->getDesPosition(), trackers[2]->getDesRotation());

				// Left or Right Forearm
				std::sort(trackers.begin() + 3, trackers.begin() + 5, sortByZAxis());
				initEndEffector(leftForeArm, trackers[3]->getDeviceIndex(), trackers[3]->getDesPosition(), trackers[3]->getDesRotation());
				initEndEffector(rightForeArm, trackers[4]->getDeviceIndex(), trackers[4]->getDesPosition(), trackers[4]->getDesRotation());
			}


		}
		else if (vrDevice.trackedDevice == TrackedDevice::Controller) {
			// Hand controller
			if (devicePos.z() > 0) {
				initEndEffector(rightHand, i, devicePos, deviceRot);
			}
			else {
				initEndEffector(leftHand, i, devicePos, deviceRot);
			}
		}
	}

	// HMD
	SensorState stateLeftEye = VrInterface::getSensorState(0);
	SensorState stateRightEye = VrInterface::getSensorState(1);
	vec3 leftEyePos = stateLeftEye.pose.vrPose.position;
	vec3 rightEyePos = stateRightEye.pose.vrPose.position;
	vec3 hmdPosCenter = (leftEyePos + rightEyePos) / 2;
	initEndEffector(head, 0, hmdPosCenter, stateLeftEye.pose.vrPose.orientation);
}

void Calibrator::gamepadButton(int buttonNr, float value) {
	//log(Info, "gamepadButton buttonNr = %i value = %f", buttonNr, value);

	// Grip button => set size and reset an avatar to a default T-Pose
	if (buttonNr == 2 && value == 1) {
		calibratedAvatar = false;
		initTransAndRot();
		avatar->resetPositionAndRotation();
		setSize();
	}

	// Menu button => calibrate
	if (buttonNr == 1 && value == 1) {
		assignControllerAndTracker();
		calibrate();
		calibratedAvatar = true;
		log(Info, "Calibrate avatar");
	}

	// Trigger button => record data
	if (buttonNr == 33 && value == 1) {
		record();
	}
}

void Calibrator::initButtons() {
	VrPoseState controller;

	int count = 0;

	for (int i = 0; i < 16; ++i) {
		controller = VrInterface::getController(i);

		if (controller.trackedDevice == TrackedDevice::Controller) {
			Gamepad::get(i)->Button = gamepadButton;
			++count;
			log(Info, "Add gamepad controller %i", count);
		}
	}

	assert(count == 2);
	controllerButtonsInitialized = true;
}
#endif
