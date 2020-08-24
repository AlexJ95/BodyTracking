#pragma once

#include "pch.h"

#include "Settings.h"
#include "InverseKinematics.h"
#include "MeshObject.h"

class AnimatedEntity : public MeshObject
{
public:
	Kore::vec3 position;
	Kore::Quaternion rotation;
	EndEffector** endEffector;
	InverseKinematics* invKin;
	bool calibrated;
	float currentHeight;
	AnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation);
	void initializeEndeffectors();
};

class Avatar : public AnimatedEntity
{
public:
	std::vector<MeshObject*> viveObjects;
	bool renderTrackerAndControllers;
	bool renderAxisForEndEffectors;
	
	Avatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation, bool renderTrackerAndController, bool renderAxisForEndEffector);
};
