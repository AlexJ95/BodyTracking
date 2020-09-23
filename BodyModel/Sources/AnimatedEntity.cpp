#include "AnimatedEntity.h"

AnimatedEntity::AnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 initialPosition, Kore::Quaternion initialRotation)
{
	meshObject = new MeshObject(meshFile, textureFile, structure, scale);
	position = initialPosition;
	rotation = initialRotation;
	invKin = new InverseKinematics(meshObject->bones);
	calibrated = false;
	// Update bones
	for (int i = 0; i < meshObject->bones.size(); ++i) invKin->initializeBone(meshObject->bones[i]);
	initializeEndeffectors();
}

AnimatedEntity::AnimatedEntity(MeshObject* meshReference, Kore::vec3 initialPosition, Kore::Quaternion initialRotation)
{
	meshObject = meshReference;
	position = initialPosition;
	rotation = initialRotation;
	invKin = new InverseKinematics(meshObject->bones);
	calibrated = false;
	// Update bones
	for (int i = 0; i < meshObject->bones.size(); ++i) invKin->initializeBone(meshObject->bones[i]);
	initializeEndeffectors();
}

//triggered if a collision was detected
void AnimatedEntity::hit()				
{
	currentHeight -= 100.0 * strength;
}

void AnimatedEntity::resetCurrentHeight()
{
	currentHeight = 100.0;
}

bool AnimatedEntity::isDead()
{
	return (currentHeight <= 0.0);
}

float AnimatedEntity::getReached() const {
	return invKin->getReached();
}
float AnimatedEntity::getStucked() const {
	return invKin->getStucked();
}

float* AnimatedEntity::getIterations() const {
	return invKin->getIterations();
}

float* AnimatedEntity::getErrorPos() const {
	return invKin->getErrorPos();
}

float* AnimatedEntity::getErrorRot() const {
	return invKin->getErrorRot();
}

float* AnimatedEntity::getTime() const {
	return invKin->getTime();
}

float* AnimatedEntity::getTimeIteration() const {
	return invKin->getTimeIteration();
}

Avatar::Avatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 initialPosition, Kore::Quaternion initialRotation, bool renderTrackerAndController, bool renderAxisForEndEffector)
	: AnimatedEntity(meshFile, textureFile, structure, scale, initialPosition, initialRotation) {
	renderTrackerAndControllers = renderTrackerAndController;
	renderAxisForEndEffectors = renderAxisForEndEffector;
	
	if (renderTrackerAndController) {
		viveObjects.emplace_back(new MeshObject("vivemodels/vivetracker.ogex", "vivemodels/", structure, 1));
		viveObjects.emplace_back(new MeshObject("vivemodels/vivecontroller.ogex", "vivemodels/", structure, 1));
	}

	if (renderTrackerAndController || renderAxisForEndEffector) {
		viveObjects.emplace_back(new MeshObject("vivemodels/axis.ogex", "vivemodels/", structure, 1));
	}
}

void Avatar::update(float deltaT)
{
	if (movementExpiration > 0) movementExpiration -= deltaT;
	else movementExpired = true;
	if (moveForward) position = position + Kore::vec3(1, 0, 0) * movementSpeed;
}

void Avatar::triggerMovement()
{
	moveForward = !moveForward;
}

void Avatar::recognizedMotion(PlayerMovement currentMovement)
{
	lastMovement = currentMovement;
	movementExpiration = 5; //to be tweaked!!!1!1!!11!elf|
	movementExpired = false;

	//Insert logic for pro-active movements here or call subroutines here
	//For example attacking or walking and jogging
}

void AnimatedEntity::initializeEndeffectors()
{
	endEffector					= new EndEffector * [numOfEndEffectors];
	endEffector[head]			= new EndEffector(headBoneIndex);
	endEffector[hip]			= new EndEffector(hipBoneIndex);
	endEffector[leftHand]		= new EndEffector(leftHandBoneIndex);
	endEffector[leftForeArm]	= new EndEffector(leftForeArmBoneIndex);
	endEffector[rightHand]		= new EndEffector(rightHandBoneIndex);
	endEffector[rightForeArm]	= new EndEffector(rightForeArmBoneIndex);
	endEffector[leftFoot]		= new EndEffector(leftFootBoneIndex);
	endEffector[rightFoot]		= new EndEffector(rightFootBoneIndex);
}