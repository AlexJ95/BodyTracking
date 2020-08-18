#include "Animator.h"

Animator::Animator() {
	math = math->getInstance();
}

void Animator::executeMovement(AnimatedEntity* entity, int endEffectorID)
{
	Kore::vec3 desPosition = entity->endEffector[endEffectorID]->getDesPosition();
	Kore::Quaternion desRotation = entity->endEffector[endEffectorID]->getDesRotation();

	// Save raw data
	//if (logRawData && typeid(entity) == typeid(Avatar)) logger->saveData(entity->endEffector[endEffectorID]->getName(), desPosition, desRotation, entity->scale);

	if (typeid(entity) != typeid(Avatar) || &static_cast<Avatar*>(entity)->calibratedAvatar) {
		// Transform desired position/rotation to the character local coordinate system
		desPosition = math->initTransInv * Kore::vec4(desPosition.x(), desPosition.y(), desPosition.z(), 1);
		desRotation = math->initRotInv.rotated(desRotation);

		// Add offset
		Kore::Quaternion offsetRotation = entity->endEffector[endEffectorID]->getOffsetRotation();
		Kore::vec3 offsetPosition = entity->endEffector[endEffectorID]->getOffsetPosition();
		Kore::Quaternion finalRot = desRotation.rotated(offsetRotation);
		Kore::vec3 finalPos = Kore::mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z()) * finalRot.matrix().Transpose() * Kore::mat4::Translation(offsetPosition.x(), offsetPosition.y(), offsetPosition.z()) * Kore::vec4(0, 0, 0, 1);

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
	BoneNode* bone = entity->bones[boneIndex - 1];
	return bone;
}

void Animator::resetPositionAndRotation(AnimatedEntity* entity) {
	for (int i = 0; i < entity->bones.size(); ++i) {
		entity->bones[i]->transform = entity->bones[i]->bind;
		entity->bones[i]->local = entity->bones[i]->bind;
		entity->bones[i]->combined = entity->bones[i]->parent->combined * entity->bones[i]->local;
		entity->bones[i]->combinedInv = entity->bones[i]->combined.Invert();
		entity->bones[i]->finalTransform = entity->bones[i]->combined * entity->bones[i]->combinedInv;
		entity->bones[i]->rotation = Kore::Quaternion(0, 0, 0, 1);
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