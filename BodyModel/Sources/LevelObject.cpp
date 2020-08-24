#include "LevelObject.h"

LevelObject::LevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 initialPosition, Kore::Quaternion initialRotation)
{
	position = initialPosition;
	rotation = initialRotation;
	meshObject = new MeshObject(meshFile, textureFile, structure, scale);
}

LevelObject::LevelObject(MeshObject* referenceMesh, Kore::vec3 initialPosition, Kore::Quaternion initialRotation)
{
	position = initialPosition;
	rotation = initialRotation;
	meshObject = referenceMesh;
}
