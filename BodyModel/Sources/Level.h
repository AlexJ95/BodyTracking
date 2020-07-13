
#include "Renderer.h"
#include "Animator.h"
#include "Logger.h"
#include "InputController.h"
#include "AudioManager.h"
#include "Calibrator.h"

#pragma once
class Level
{
public:
	struct LevelObject
	{
		Renderer::LevelObject* render;
		LevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale);
	};

	struct HumanoidEntity
	{
		EndEffector** endEffector;
		Renderer::HumanoidEntity* entity;
		Animator* animator;

		HumanoidEntity(); // empty default constructor
		HumanoidEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale);
		void initEndeffectors();
	};

	struct Avatar : public HumanoidEntity
	{
		bool calibratedAvatar;
		Calibrator* calibrator;
		Avatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, bool renderTrackerAndController, bool renderAxisForEndEffector, float scale);
	};

	Renderer* renderer;
	std::vector<LevelObject> environment;
	std::vector<HumanoidEntity> enemies;
	Avatar* avatar;
	InputController* input;
	AudioManager* audio;

	virtual void update(double deltaT);

	virtual void init();
};

