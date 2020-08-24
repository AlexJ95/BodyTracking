#include "Level.h"

Level::Level()
{
	renderer = new Renderer();
	animator = new Animator();
	math = math->getInstance();
}

void Level::update(double deltaT)
{
	renderer->update(deltaT);
	for (NonPlayerCharacter* entity : enemies) entity->ai->update(deltaT, math->cameraPos);
}

void Level::init()
{
	std::vector<LevelObject*> objects;
	for (ALevelObject* object : environment) objects.emplace_back(object->render);
	std::vector<AnimatedEntity*> entities;
	for (NonPlayerCharacter* entity : enemies) entities.emplace_back(entity->entity);
	renderer->init(objects, entities, avatar->entity, animator);
}

Level::ALevelObject::ALevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale)
{
	render = new LevelObject(meshFile, textureFile, structure, scale);
}

Level::AnAnimatedEntity::AnAnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation)
{
	entity = new AnimatedEntity(meshFile, textureFile, structure, scale, position, rotation);
}

Level::TheAvatar::TheAvatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation, bool renderTrackerAndController, bool renderAxisForEndEffector)
{
	entity = new Avatar(meshFile, textureFile, structure, scale, position, rotation, renderTrackerAndController, renderAxisForEndEffector);
}

Level::NonPlayerCharacter::NonPlayerCharacter(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation) : Level::AnAnimatedEntity(meshFile, textureFile, structure, scale, position, rotation)
{
	
}
