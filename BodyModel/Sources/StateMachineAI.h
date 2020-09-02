#pragma once

#include "CustomMath.h"
#include "Animator.h"

class StateMachineAI
{
protected:
	Animator* animator;
	AnimatedEntity* entity;
	Logger* logger;
	bool inAnimation;
	Kore::mat4 locToGlob = Kore::mat4::RotationY(0.5 * Kore::pi) * Kore::mat4::RotationX(-0.5 * Kore::pi);

	float maxDistanceToEnemy = 0.5f;
	Kore::vec3 currentPosOtherEnemy;
	bool tooClose = false;

public:
	StateMachineAI(AnimatedEntity* enemyEntity, Animator* animatorReference);
	//virtual ~StateMachineAI() = 0;

	//the different states of the AI state machine
	enum class AIState;
	StateMachineAI::AIState currentState;


	std::map <StateMachineAI::AIState, StateMachineAI::AIState(StateMachineAI::*)(float deltaT, Kore::vec3 playerPosition)> stateToAction;
	typedef AIState(StateMachineAI::* action)(float deltaT, Kore::vec3 playerPosition);
	
	std::map <string, const char*> animationLibrary;

	void update(float deltaT, Kore::vec3 playerPosition);
	void spawn();
	void respawn();
	void hit();						//vielleicht verschiedene Attacken einbauen
	void checkColision(Kore::vec3 posOtherEnemy);
	
};

class CyborgAI : public StateMachineAI
{
private:
	float radians = 0.0;	//orientation bezween player and nonPlayerCharacter
	bool attack = false;
public:
	float dRot = 0.3;
	float dRotCol = 0.1;
	float dTrans = 0.02;
	float maxDistanceToPlayer = 1.0f;
	float limitPosX = 1.5;

	static int numberOfAttackers;
	int maxAttackers = 2;

	enum class AIState { Attacking, Pursueing, Planning };
	
	AIState attacking(float deltaT, Kore::vec3 playerPosition);
	AIState pursueing(float deltaT, Kore::vec3 playerPosition);
	AIState planning(float deltaT, Kore::vec3 playerPosition);

	CyborgAI(AnimatedEntity* enemyEntity, Animator* animatorReference);
	
	//~CyborgAI();
};
