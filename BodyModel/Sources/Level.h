#pragma once

#include "Renderer.h"
#include "Animator.h"
#include "InputController.h"
#include "AudioManager.h"
#include "Calibrator.h"
#include "LevelObject.h"
#include "AnimatedEntity.h"

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
		AnAnimatedEntity(); // empty default constructor
		AnAnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale);
	};

	struct TheAvatar : public AnAnimatedEntity
	{
		bool calibratedAvatar;
		TheAvatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, bool renderTrackerAndController, bool renderAxisForEndEffector);
	};

	Renderer* renderer;
	std::vector<ALevelObject> environment;
	std::vector<AnAnimatedEntity> enemies;
	TheAvatar* avatar;
	InputController* input;
	AudioManager* audio;
	Calibrator* calibrator;
	Animator* animator;

	virtual void update(double deltaT);

	virtual void init();
};

