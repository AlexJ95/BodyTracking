#include "Level.h"

void Level::update(double deltaT)
{
	renderer->update(deltaT);
	input->update(deltaT);
}

void Level::init()
{
	std::vector<Renderer::LevelObject> objects;
	for (LevelObject object : environment) objects.emplace_back(object.render);
	std::vector<Renderer::HumanoidEntity> entities;
	for (HumanoidEntity entity : enemies) entities.emplace_back(entity.entity);
	entities.emplace_back(avatar->entity);
	renderer = new Renderer(objects, entities);
	input->init();
}

Level::LevelObject::LevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale) {
	render = new Renderer::LevelObject(meshFile, textureFile, structure, scale);
}

void Level::HumanoidEntity::initEndeffectors()
{
	endEffector = new EndEffector * [numOfEndEffectors];
	endEffector[head] = new EndEffector(headBoneIndex);
	endEffector[hip] = new EndEffector(hipBoneIndex);
	endEffector[leftHand] = new EndEffector(leftHandBoneIndex);
	endEffector[leftForeArm] = new EndEffector(leftForeArmBoneIndex);
	endEffector[rightHand] = new EndEffector(rightHandBoneIndex);
	endEffector[rightForeArm] = new EndEffector(rightForeArmBoneIndex);
	endEffector[leftFoot] = new EndEffector(leftFootBoneIndex);
	endEffector[rightFoot] = new EndEffector(rightFootBoneIndex);
}

Level::HumanoidEntity::HumanoidEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale)
{
	initEndeffectors();
	entity = new Renderer::HumanoidEntity(meshFile, textureFile, structure, scale, animator);
	animator = new Animator(entity);
}

Level::Avatar::Avatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, bool renderTrackerAndController, bool renderAxisForEndEffector, float scale)
{
	initEndeffectors();
	entity = new Renderer::Avatar(meshFile, textureFile, structure, scale, animator, renderTrackerAndController, renderAxisForEndEffector);
	animator = new Animator(entity);
	calibrator = new Calibrator(animator);
}
