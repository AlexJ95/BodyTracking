#pragma once

#include "Renderer.h"
#include "Animator.h"
#include "InputController.h"
#include "AudioManager.h"
#include "LevelObject.h"
#include "AnimatedEntity.h"
#include "StateMachineAI.h"
#include "CustomMath.h"
#include "EndEffector.h"
#include "UI3D.h"
#include "Form.h"



class Level
{
public:
	struct ALevelObject
	{
		//Relatively empty and redundant at the moment, however this space is supposed to hold all potential components for levelObject like new hazard or maybe collectible components
		LevelObject* object;
		Kore::vec3 initPosition;
		//Kore::Quaternion rotation;
		ALevelObject(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation);
		ALevelObject(ALevelObject* reference, Kore::vec3 position, Kore::Quaternion rotation);
	};

	struct AnAnimatedEntity
	{
		AnimatedEntity* entity;
		std::string tag;
		AnAnimatedEntity() {}; // empty default constructor
		AnAnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation);
		AnAnimatedEntity(AnAnimatedEntity* reference, Kore::vec3 position, Kore::Quaternion rotation);
	};

	struct TheAvatar : public AnAnimatedEntity
	{
		Avatar* entity;
		TheAvatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation, bool renderTrackerAndController, bool renderAxisForEndEffector);
	};

	struct NonPlayerCharacter : public AnAnimatedEntity
	{
		StateMachineAI* ai;
		NonPlayerCharacter(AnAnimatedEntity* reference, Kore::vec3 position, Kore::Quaternion rotation);
	};

	//Actual content of the level
	std::vector<ALevelObject*> environment;
	std::vector<NonPlayerCharacter*> enemies;
	TheAvatar* avatar;

	//Controls and audio singleton references
	InputController* input;
	AudioManager* audio;

	//Other essential tools
	Renderer* renderer;
	Animator* animator;
	Form* form = NULL;
	CustomMath* math;

	Level();

	virtual void update(double deltaT);
	virtual void init();
	virtual void setUI(UI3D* ui);

	virtual void controlsSetup();
	virtual void audioSetup();
	virtual void graphicsSetup();

	virtual void x() {}


};

