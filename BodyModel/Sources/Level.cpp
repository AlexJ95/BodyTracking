#include "Level.h"

Level::Level() {
	renderer = new Renderer();
	animator = new Animator();
}

void Level::update(double deltaT)
{
	input->update(deltaT);
	renderer->update(deltaT);
}

void Level::init()
{
	input->init();
	std::vector<LevelObject*> objects;
	for (ALevelObject* object : environment) objects.emplace_back(object->render);
	std::vector<AnimatedEntity*> entities;
	for (AnAnimatedEntity* entity : enemies) entities.emplace_back(entity->entity);
	renderer->init(objects, entities, avatar->entity, animator);
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
