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
	bool inAnimation;
	Kore::mat4 locToGlob = Kore::mat4::RotationY(0.5 * Kore::pi) * Kore::mat4::RotationX(-0.5 * Kore::pi);

	float maxDistanceToEnemy = 0.8f;
	Kore::vec3 currentPosOtherEnemy;
	bool tooClose = false;

public:
	StateMachineAI(AnimatedEntity* enemyEntity, Animator* animatorReference, Avatar* avatarReference);
	//virtual ~StateMachineAI() = 0;

	//the different states of the AI state machine
	enum class AIState;
	StateMachineAI::AIState currentState;


	std::map <StateMachineAI::AIState, StateMachineAI::AIState(StateMachineAI::*)(float deltaT)> stateToAction;
	typedef AIState(StateMachineAI::* action)(float deltaT);
	
	std::map <string, const char*> animationLibrary;

	void update(float deltaT);
	void spawn();
	void checkCollision(Kore::vec3 posOtherEnemy);
	static int beatedEnemyCount;
	static float lastDeadPos;
	
};

class CyborgAI : public StateMachineAI
{
private:
	float radians = Kore::pi;	//orientation bezween player and nonPlayerCharacter
	bool attack = false;
public:
	float dRot = 0.3;
	float dRotCol = 0.08;
	float dTrans = 0.02;
	float maxDistanceToPlayer = 1.0f;
	float limitPosX = 1.5;
	bool died;

	enum class AIState { Attacking, Pursueing, Planning, Dying };
	
	AIState attacking	(float deltaT);
	AIState pursueing	(float deltaT);
	AIState planning	(float deltaT);
	AIState dying		(float deltaT);

	CyborgAI(AnimatedEntity* enemyEntity, Animator* animatorReference, Avatar* avatarReference);
	
	//~CyborgAI();
};
