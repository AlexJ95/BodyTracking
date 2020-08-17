#include "AnimatedEntity.h"

AnimatedEntity::AnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale)
	: MeshObject(meshFile, textureFile, structure, scale) 
{
	invKin = new InverseKinematics(bones);
	// Update bones
	for (int i = 0; i < bones.size(); ++i) invKin->initializeBone(bones[i]);
	initializeEndeffectors();
}

Avatar::Avatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, bool renderTrackerAndController, bool renderAxisForEndEffector)
	: AnimatedEntity(meshFile, textureFile, structure, scale) {
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

void AnimatedEntity::initializeEndeffectors()
{
	endEffector = new EndEffector * [numOfEndEffectors];
	endEffector[head] = new EndEffector(headBoneIndex);
	endEffector[hip] = new EndEffector(hipBoneIndex);
	endEffector[leftHand] = new EndEffector(leftHandBoneIndex);
	endEffector[leftForeArm] = new EndEffector(leftForeArmBoneIndex);
	endEffector[rightHand] = new EndEffector(rightHandBoneIndex);
	endEffector[rightForeArm] = new EndEffector(rightForeArmBoneIndex);
	endEffector[leftFoot] = new EndEffector(leftFootBoneIndex);
	endEffector[rightFoot] = new EndEffector(rightFootBoneIndex);
}
