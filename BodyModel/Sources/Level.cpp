#include "Level.h"

void Level::update(double deltaT)
{
	renderer->update(deltaT);
	input->update(deltaT);
}

void Level::init()
{
	std::vector<LevelObject*> objects;
	for (ALevelObject* object : environment) objects.emplace_back(object->render);
	std::vector<AnimatedEntity*> entities;
	for (AnAnimatedEntity* entity : enemies) entities.emplace_back(entity->entity);
	entities.emplace_back(avatar->entity);
	renderer->init(objects, entities, animator);
	input->init();
}

Level::ALevelObject::ALevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale) {
	render = new LevelObject(meshFile, textureFile, structure, scale);
}

Level::AnAnimatedEntity::AnAnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale)
{
	entity = new AnimatedEntity(meshFile, textureFile, structure, scale);
}

Level::TheAvatar::TheAvatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, bool renderTrackerAndController, bool renderAxisForEndEffector)
{
	entity = new Avatar(meshFile, textureFile, structure, scale, renderTrackerAndController, renderAxisForEndEffector);
}
