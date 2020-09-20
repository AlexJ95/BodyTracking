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
	renderer->form = form;
	

	for (NonPlayerCharacter* entity : enemies) entity->ai->update(deltaT);
}

void Level::init()
{	
	for (ALevelObject* object : environment) objects.emplace_back(object->render);
	std::vector<AnimatedEntity*> entities;
	for (NonPlayerCharacter* entity : enemies) entities.emplace_back(entity->entity);
	renderer->init(objects, entities, avatar->entity, animator);
	input->setAnimatorAndAvatar(animator, avatar->entity);
}

void Level::setUI(UI3D* ui)
{
	renderer->ui = ui;
}

Level::ALevelObject::ALevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation)
{
	render = new LevelObject(meshFile, textureFile, structure, scale, position, rotation);
}

Level::ALevelObject::ALevelObject(ALevelObject* reference, Kore::vec3 position, Kore::Quaternion rotation)
{
	render = new LevelObject(reference->render->meshObject, position, rotation);
	render->activated = reference->render->activated;
	render->moveable = reference->render->moveable;
	render->tag = reference->render->tag;
}

Level::AnAnimatedEntity::AnAnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation)
{
	entity = new AnimatedEntity(meshFile, textureFile, structure, scale, position, rotation);
}

Level::AnAnimatedEntity::AnAnimatedEntity(AnAnimatedEntity* reference, Kore::vec3 position, Kore::Quaternion rotation)
{
	entity = new AnimatedEntity(reference->entity->meshObject, position, rotation);
	entity->colorTag = reference->tag;
}

Level::TheAvatar::TheAvatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation, bool renderTrackerAndController, bool renderAxisForEndEffector)
{
	entity = new Avatar(meshFile, textureFile, structure, scale, position, rotation, renderTrackerAndController, renderAxisForEndEffector);
}

Level::NonPlayerCharacter::NonPlayerCharacter(AnAnimatedEntity* reference, Kore::vec3 position, Kore::Quaternion rotation) : Level::AnAnimatedEntity(reference, position, rotation)
{
	
}
