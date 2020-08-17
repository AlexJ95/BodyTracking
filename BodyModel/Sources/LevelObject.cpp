#include "LevelObject.h"

LevelObject::LevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale)
	: MeshObject(meshFile, textureFile, structure, scale) { }