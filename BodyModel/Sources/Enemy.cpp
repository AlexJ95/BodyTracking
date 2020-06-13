#include "Enemy.h"
#include "Settings.h"
#include "Logger.h"

const int numOfEndEffectors = 8;
Kore::vec3 desPosition[numOfEndEffectors];
Kore::Quaternion desRotation[numOfEndEffectors];
Logger* logger;
EndEffector** endEffector;
Kore::mat4 initTrans;
Kore::mat4 initTransInv;
Kore::Quaternion initRot;
Kore::Quaternion initRotInv;

void Enemy::act()
{
	switch (currentState) {
	case Attacking :
		attack();
	case Defending :
		defend();
	case Approaching :
		approach();
	case Planning :
		plan();
	}
}

//attack player once and immediatly return to planning
void Enemy::attack()
{
	if (!logger->readData(numOfEndEffectors, files[4], desPosition, desRotation, scaleFactor, 0))
		currentState = Planning;
	for (int i = 0; i < numOfEndEffectors; ++i) {
		endEffector[i]->setDesPosition(desPosition[i]);
		endEffector[i]->setDesRotation(desRotation[i]);
	}
	for (int i = 0; i < numOfEndEffectors; ++i) executeMovement(i);
}

//TBD in the future
void Enemy::defend()
{
	currentState = Planning;
}

//Approach player until entering combat distance
void Enemy::approach()
{
	currentState = Planning;
}

//this method incorporates the actual decision making
void Enemy::plan()
{

}


//The following two methods are copied from main
//Main is kinda a mess and should be tidied up along with the two methods below
void Enemy::executeMovement(int endEffectorID)
{
	Kore::vec3 desPosition = endEffector[endEffectorID]->getDesPosition();
	Kore::Quaternion desRotation = endEffector[endEffectorID]->getDesRotation();

	desPosition = initTransInv * Kore::vec4(desPosition.x(), desPosition.y(), desPosition.z(), 1);
	desRotation = initRotInv.rotated(desRotation);

	// Add offset
	Kore::Quaternion offsetRotation = endEffector[endEffectorID]->getOffsetRotation();
	Kore::vec3 offsetPosition = endEffector[endEffectorID]->getOffsetPosition();
	Kore::Quaternion finalRot = desRotation.rotated(offsetRotation);
	Kore::vec3 finalPos = Kore::mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z()) * finalRot.matrix().Transpose() * mat4::Translation(offsetPosition.x(), offsetPosition.y(), offsetPosition.z()) * vec4(0, 0, 0, 1);

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

void initTransAndRot() {
	initRot = Kore::Quaternion(0, 0, 0, 1);
	initRot.rotate(Kore::Quaternion(Kore::vec3(1, 0, 0), -Kore::pi / 2.0));
	initRot.rotate(Kore::Quaternion(Kore::vec3(0, 0, 1), Kore::pi / 2.0));
	initRot.normalize();
	initRotInv = initRot.invert();

	Kore::vec3 initPos = Kore::vec4(0, 0, 0, 1);
	initTrans = Kore::mat4::Translation(initPos.x(), initPos.y(), initPos.z()) * initRot.matrix().Transpose();
	initTransInv = initTrans.Invert();
}
