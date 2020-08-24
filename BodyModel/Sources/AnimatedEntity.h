#pragma once

#include "pch.h"

#include "Settings.h"
#include "InverseKinematics.h"
#include "MeshObject.h"

class AnimatedEntity
{
public:
	Kore::vec3 position;
	Kore::Quaternion rotation;
	EndEffector** endEffector;
	InverseKinematics* invKin;
	bool calibrated;
	float currentHeight;
	MeshObject* meshObject;
	AnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 initialPosition, Kore::Quaternion initialRotation);
	AnimatedEntity(MeshObject* meshReference, Kore::vec3 initialPosition, Kore::Quaternion initialRotation);
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
