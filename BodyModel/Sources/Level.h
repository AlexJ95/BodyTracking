#pragma once

#include "Renderer.h"
#include "Animator.h"
#include "InputController.h"
#include "AudioManager.h"
#include "Calibrator.h"
#include "LevelObject.h"
#include "AnimatedEntity.h"
#include "StateMachineAI.h"
#include "CustomMath.h"

class Level
{
public:
	struct ALevelObject
	{
		//Relatively empty and redundant at the moment, however this space is supposed to hold all potential components for levelObject like new hazard or maybe collectible components
		LevelObject* render;
		ALevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale);
	};

	struct AnAnimatedEntity
	{
		AnimatedEntity* entity;
		AnAnimatedEntity() {}; // empty default constructor
		AnAnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation);
	};

	struct TheAvatar : public AnAnimatedEntity
	{
		Avatar* entity;
		TheAvatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation, bool renderTrackerAndController, bool renderAxisForEndEffector);
	};

	struct NonPlayerCharacter : public AnAnimatedEntity
	{
		StateMachineAI* ai;
		NonPlayerCharacter(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation);
	};

	Renderer* renderer;
	std::vector<ALevelObject*> environment;
	std::vector<NonPlayerCharacter*> enemies;
	TheAvatar* avatar;
	InputController* input;
	AudioManager* audio;
	Calibrator* calibrator;
	Animator* animator;
	CustomMath* math;

	Level();

	virtual void update(double deltaT);

	virtual void init();
};

