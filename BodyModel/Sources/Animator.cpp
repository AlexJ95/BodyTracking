#include "Animator.h"

Animator::Animator() {}

Animator::Animator(Renderer::HumanoidEntity* humanoidEntity) {
	math->getInstance();
	entity = humanoidEntity;
	endEffector = entity->endEffector;

	invKin = new InverseKinematics(entity->bones);
	
	// Update bones
	for (int i = 0; i < entity->bones.size(); ++i) invKin->initializeBone(entity->bones[i]);

	// Get the highest position
	BoneNode* head = getBoneWithIndex(headBoneIndex);
	Kore::vec4 position = head->combined * Kore::vec4(0, 0, 0, 1);
	position *= 1.0 / position.w();
	currentHeight = position.z();
}

void Animator::executeMovement(int endEffectorID)
{
	Kore::vec3 desPosition = endEffector[endEffectorID]->getDesPosition();
	Kore::Quaternion desRotation = endEffector[endEffectorID]->getDesRotation();

	// Save raw data
	if (logRawData && typeid(entity) == typeid(Renderer::Avatar)) logger->saveData(endEffector[endEffectorID]->getName(), desPosition, desRotation, entity->scale);

	if (typeid(entity) != typeid(Renderer::Avatar) || &static_cast<Renderer::Avatar*>(entity)->calibratedAvatar) {
		// Transform desired position/rotation to the character local coordinate system
		desPosition = math->initTransInv * vec4(desPosition.x(), desPosition.y(), desPosition.z(), 1);
		desRotation = math->initRotInv.rotated(desRotation);

		// Add offset
		Kore::Quaternion offsetRotation = endEffector[endEffectorID]->getOffsetRotation();
		vec3 offsetPosition = endEffector[endEffectorID]->getOffsetPosition();
		Kore::Quaternion finalRot = desRotation.rotated(offsetRotation);
		vec3 finalPos = mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z()) * finalRot.matrix().Transpose() * mat4::Translation(offsetPosition.x(), offsetPosition.y(), offsetPosition.z()) * vec4(0, 0, 0, 1);

		if (endEffectorID == hip) {
			setFixedPositionAndOrientation(endEffector[endEffectorID]->getBoneIndex(), finalPos, finalRot);
		}
		else if (endEffectorID == head || endEffectorID == leftForeArm || endEffectorID == rightForeArm || endEffectorID == leftFoot || endEffectorID == rightFoot) {
			setDesiredPositionAndOrientation(endEffector[endEffectorID]->getBoneIndex(), endEffector[endEffectorID]->getIKMode(), finalPos, finalRot);
		}
		else if (endEffectorID == leftHand || endEffectorID == rightHand) {
			setFixedOrientation(endEffector[endEffectorID]->getBoneIndex(), finalRot);
		}
	}
}

void Animator::setDesiredPositionAndOrientation(int boneIndex, IKMode ikMode, Kore::vec3 desPosition, Kore::Quaternion desRotation) {
	BoneNode* bone = getBoneWithIndex(boneIndex);

	invKin->inverseKinematics(bone, ikMode, desPosition, desRotation);
}

void Animator::setFixedPositionAndOrientation(int boneIndex, Kore::vec3 desPosition, Kore::Quaternion desRotation) {
	BoneNode* bone = getBoneWithIndex(boneIndex);

	bone->transform = mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z());
	bone->rotation = desRotation;
	bone->rotation.normalize();
	bone->local = bone->transform * bone->rotation.matrix().Transpose();
}

void Animator::setFixedOrientation(int boneIndex, Kore::Quaternion desRotation) {
	BoneNode* bone = getBoneWithIndex(boneIndex);

	Kore::Quaternion localRot;
	Kore::RotationUtility::getOrientation(&bone->parent->combined, &localRot);
	bone->rotation = localRot.invert().rotated(desRotation);

	//bone->transform = mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z());
	//bone->rotation = desRotation;

	bone->rotation.normalize();
	bone->local = bone->transform * bone->rotation.matrix().Transpose();
}

BoneNode* Animator::getBoneWithIndex(int boneIndex) const {
	BoneNode* bone = entity->bones[boneIndex - 1];
	return bone;
}

void Animator::resetPositionAndRotation() {
	for (int i = 0; i < entity->bones.size(); ++i) {
		entity->bones[i]->transform = entity->bones[i]->bind;
		entity->bones[i]->local = entity->bones[i]->bind;
		entity->bones[i]->combined = entity->bones[i]->parent->combined * entity->bones[i]->local;
		entity->bones[i]->combinedInv = entity->bones[i]->combined.Invert();
		entity->bones[i]->finalTransform = entity->bones[i]->combined * entity->bones[i]->combinedInv;
		entity->bones[i]->rotation = Kore::Quaternion(0, 0, 0, 1);
	}
}

float Animator::getReached() const {
	return invKin->getReached();
}

float Animator::getStucked() const {
	return invKin->getStucked();
}

float* Animator::getIterations() const {
	return invKin->getIterations();
}

float* Animator::getErrorPos() const {
	return invKin->getErrorPos();
}

float* Animator::getErrorRot() const {
	return invKin->getErrorRot();
}

float* Animator::getTime() const {
	return invKin->getTime();
}

float* Animator::getTimeIteration() const {
	return invKin->getTimeIteration();
}

float Animator::getHeight() const {
	return currentHeight;
}