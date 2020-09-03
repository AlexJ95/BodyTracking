#pragma once

#include "pch.h"

#include "Settings.h"
#include "InverseKinematics.h"
#include "MeshObject.h"

class AnimatedEntity
{
private:
	float currentHeight;
public:
	Kore::vec3 position;
	Kore::Quaternion rotation;
	EndEffector** endEffector;
	InverseKinematics* invKin;
	bool activated=false;
	bool calibrated;

	float strength = 1.0;	//strength=1.0 -> mit einem Schlag ist di Entity tod
	MeshObject* meshObject;
	AnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 initialPosition, Kore::Quaternion initialRotation);
	AnimatedEntity(MeshObject* meshReference, Kore::vec3 initialPosition, Kore::Quaternion initialRotation);
	void initializeEndeffectors();
	void hit();
	void resetCurrentHeight();
	float getCurrentHeight();
	bool attackingSucceed = false;
};

class Avatar : public AnimatedEntity
{
public:
	std::vector<MeshObject*> viveObjects;
	bool renderTrackerAndControllers;
	bool renderAxisForEndEffectors;


	Avatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation, bool renderTrackerAndController, bool renderAxisForEndEffector);
};
