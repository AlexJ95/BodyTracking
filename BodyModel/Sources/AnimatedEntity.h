#pragma once

#include "Settings.h"
#include "MeshObject.h"
#include "CustomMath.h"
#include "InverseKinematics.h"

class AnimatedEntity : public MeshObject
{
public:
	EndEffector** endEffector;
	InverseKinematics* invKin;
	float currentHeight;
	AnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale);
	void initializeEndeffectors();
};

class Avatar : public AnimatedEntity
{
public:
	std::vector<MeshObject*> viveObjects;
	bool renderTrackerAndControllers;
	bool renderAxisForEndEffectors;
	bool calibratedAvatar;

	Avatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, bool renderTrackerAndController, bool renderAxisForEndEffector);
};
