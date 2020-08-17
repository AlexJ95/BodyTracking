#pragma once

#include "Settings.h"
#include "MeshObject.h"
#include "Animator.h"

class LevelObject : public MeshObject
{
public:
	LevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale = 1.0f);
};

