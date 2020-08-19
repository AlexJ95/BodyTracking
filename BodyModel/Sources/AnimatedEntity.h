#pragma once

#include "pch.h"

#include "Settings.h"
#include "InverseKinematics.h"
#include "MeshObject.h"

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
