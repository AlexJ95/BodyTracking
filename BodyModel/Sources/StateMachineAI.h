#pragma once

#include "CustomMath.h"
#include "Animator.h"

class StateMachineAI
{
	Animator* animator;
	AnimatedEntity* entity;
	bool inAnimation;

public:
	StateMachineAI(AnimatedEntity* enemyEntity, Animator* animatorReference);
	virtual ~StateMachineAI() = 0;

	//the different states of the AI state machine
	enum class AIState;
	AIState currentState;
	typedef AIState (*action)(float deltaT, Kore::vec3 playerPosition);
	std::map < AIState, AIState(StateMachineAI::*)(float deltaT, Kore::vec3 playerPosition) > stateToAction;

	void update(float deltaT);
};

class CyborgAI : public StateMachineAI
{
public:
	enum class AIState { Attacking, Pursueing, Planning };
	
	AIState attacking(float deltaT, Kore::vec3 playerPosition);
	//AIStates pursueing();
	//AIStates planning();

	//void update(float deltaT, Kore::vec3 playerPosition);

	CyborgAI(AnimatedEntity* enemyEntity, Animator* animatorReference);
	//~CyborgAI();
};

