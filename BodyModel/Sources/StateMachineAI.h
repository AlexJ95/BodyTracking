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
};

class CyborgAI : public StateMachineAI
{
public:
	enum class AIState { Attacking, Pursueing, Planning };
	
	AIState attacking(float deltaT, Kore::vec3 playerPosition);
	AIState pursueing(float deltaT, Kore::vec3 playerPosition);
	AIState planning(float deltaT, Kore::vec3 playerPosition);

	CyborgAI(AnimatedEntity* enemyEntity, Animator* animatorReference);
	//~CyborgAI();
};
