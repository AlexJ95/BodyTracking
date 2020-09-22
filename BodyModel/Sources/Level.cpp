#include "Level.h"

Level::Level()
{
	renderer = new Renderer();
	
	math = math->getInstance();
}

void Level::update(double deltaT)
{	
	renderer->form = form;
	if (form->isFormShown())
		audio->play("titleSong");

	animator->rigVrPose(avatar->entity);
	for (NonPlayerCharacter* entity : enemies) entity->ai->update(deltaT);
}

void Level::init()
{	
	std::vector<LevelObject*> objects;
	for (ALevelObject* object : environment) objects.emplace_back(object->object);
	std::vector<AnimatedEntity*> entities;
	for (NonPlayerCharacter* entity : enemies) entities.emplace_back(entity->entity);
	renderer->init(objects, entities, avatar->entity, animator);
	input->setAnimatorAndAvatar(animator, avatar->entity);
}

void Level::setUI(UI3D* ui)
{
	renderer->ui = ui;
}

void Level::controlsSetup()
{

}

void Level::audioSetup()
{

}

void Level::graphicsSetup()
{

}

Level::ALevelObject::ALevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation)
{
	object = new LevelObject(meshFile, textureFile, structure, scale, position, rotation);
}

Level::ALevelObject::ALevelObject(ALevelObject* reference, Kore::vec3 position, Kore::Quaternion rotation)
{
	object = new LevelObject(reference->object->meshObject, position, rotation);
	object->activated = reference->object->activated;
	object->moveable = reference->object->moveable;
	object->tag = reference->object->tag;
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
