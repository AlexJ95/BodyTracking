#pragma once

#include "pch.h"

#include "Settings.h"
#include "MeshObject.h"

class LevelObject
{
public:
	Kore::vec3 position;
	Kore::Quaternion rotation;
	MeshObject* meshObject;
	LevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 initialPosition, Kore::Quaternion initialRotation);
	LevelObject(MeshObject* referenceMesh, Kore::vec3 initialPosition, Kore::Quaternion initialRotation);
};

