#include "Animator.h"

Animator::Animator() {
	math = math->getInstance();
	calibrator = new Calibrator();
}

bool Animator::executeAnimation(AnimatedEntity* entity, const char* filename, Logger* logger)
{
	float scaleFactor;
	Kore::vec3 desPosition[numOfEndEffectors];
	Kore::Quaternion desRotation[numOfEndEffectors];
	bool inAnimation = logger->readData(numOfEndEffectors, filename, desPosition, desRotation, scaleFactor);

	for (int i = 0; i < numOfEndEffectors; ++i) {
		entity->endEffector[i]->setDesPosition(desPosition[i]);
		entity->endEffector[i]->setDesRotation(desRotation[i]);
	}

	
	if (!entity->calibrated) {
		resetPositionAndRotation(entity);
		entity->meshObject->setScale(scaleFactor);
		BoneNode* bones[numOfEndEffectors];
		for (int i = 0; i < numOfEndEffectors; i++) bones[i] = getBoneWithIndex(entity, entity->endEffector[i]->getBoneIndex());
		calibrator->calibrate(entity, bones);
		entity->calibrated = true;
	}
	
	for (int i = 0; i < numOfEndEffectors; ++i) executeMovement(entity, i);

	entity->calibrated = inAnimation;
	return inAnimation;
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
		desPosition = locTransInv * math->initTransInv * Kore::vec4(desPosition.x(), desPosition.y(), desPosition.z(), 1);
		desRotation = locRotInv.rotated(math->initRotInv.rotated(desRotation));

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

float Animator::getReached(AnimatedEntity* entity) const {
	return entity->invKin->getReached();
}

float Animator::getStucked(AnimatedEntity* entity) const {
	return entity->invKin->getStucked();
}

float* Animator::getIterations(AnimatedEntity* entity) const {
	return entity->invKin->getIterations();
}

float* Animator::getErrorPos(AnimatedEntity* entity) const {
	return entity->invKin->getErrorPos();
}

float* Animator::getErrorRot(AnimatedEntity* entity) const {
	return entity->invKin->getErrorRot();
}

float* Animator::getTime(AnimatedEntity* entity) const {
	return entity->invKin->getTime();
}

float* Animator::getTimeIteration(AnimatedEntity* entity) const {
	return entity->invKin->getTimeIteration();
}

float Animator::getCurrentHeight(AnimatedEntity* entity) const {
	// Get the highest position
	BoneNode* head = getBoneWithIndex(entity, headBoneIndex);
	Kore::vec4 position = head->combined * Kore::vec4(0, 0, 0, 1);
	position *= 1.0 / position.w();
	return position.z();
}