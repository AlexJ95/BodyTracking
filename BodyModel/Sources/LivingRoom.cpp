#include "pch.h"
#include "LivingRoom.h"

using namespace Kore;
using namespace Kore::Graphics4;

LivingRoom::LivingRoom(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale) : MeshObject(meshFile, textureFile, structure, scale) {
	
}

void LivingRoom::render(TextureUnit tex, Kore::Graphics4::ConstantLocation mLocation, Kore::Graphics4::ConstantLocation mLocationInverse, ConstantLocation diffuseLocation, ConstantLocation specularLocation, ConstantLocation specularPowerLocation, bool mirror) {
	
}

void LivingRoom::setLights(Kore::Graphics4::ConstantLocation lightCountLocation, Kore::Graphics4::ConstantLocation lightPosLocation) {
	
}
