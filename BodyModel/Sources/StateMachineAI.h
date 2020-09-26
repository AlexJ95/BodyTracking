#pragma once

#include "CustomMath.h"
#include "Animator.h"

class StateMachineAI
{
protected:
	Animator* animator;
	AnimatedEntity* entity;
	Avatar* avatar;
	Logger* logger;
	string currentAnimation;
	bool inAnimation;
	Kore::vec3 posToMove;
	float speed = 4.0f;
	int dead = 10;

public:
	StateMachineAI(AnimatedEntity* enemyEntity, Animator* animatorReference, Avatar* avatarReference);

	//the different states of the AI state machine
	enum class AIState;
	StateMachineAI::AIState currentState;
	StateMachineAI::AIState lastState;


	std::map <StateMachineAI::AIState, StateMachineAI::AIState(StateMachineAI::*)(double deltaT)> stateToAction;
	typedef AIState(StateMachineAI::* action)(double deltaT);
	
	std::map <string, const char*> animationLibrary;

	void update(double deltaT);
	bool continueMovement(double deltaT);

	Kore::vec3 toInvertPos(Kore::vec3 pos);

	Kore::vec3 toOriginPos(Kore::vec3 pos);

	void setEntityPos(Kore::vec3 pos);

};

class CyborgAI : public StateMachineAI
{
private:

public:

	enum class AIState { Attacking, Pursueing, Planning, Dying, Falling, Landing};
	
	//states:
	AIState attacking	(double deltaT);
	AIState pursueing	(double deltaT);
	AIState planning	(double deltaT);
	AIState dying		(double deltaT);
	AIState falling		(double deltaT);
	AIState landing		(double deltaT);

	CyborgAI(AnimatedEntity* enemyEntity, Animator* animatorReference, Avatar* avatarReference);
	

};
