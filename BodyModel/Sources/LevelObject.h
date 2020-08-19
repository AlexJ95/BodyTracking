#pragma once

#include "pch.h"

#include "Settings.h"
#include "MeshObject.h"

class LevelObject : public MeshObject
{
public:
	LevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale = 1.0f);
};

