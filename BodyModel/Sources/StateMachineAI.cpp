#include "StateMachineAI.h"

void StateMachineAI::update(float deltaT, Kore::vec3 playerPosition)
{
	currentState = (this->*stateToAction.at(currentState))(deltaT, playerPosition);
}

StateMachineAI::StateMachineAI(AnimatedEntity* enemyEntity, Animator* animatorReference)
{
	entity = enemyEntity;
	animator = animatorReference;
	logger = new Logger();
	inAnimation = false;
}

CyborgAI::AIState CyborgAI::attacking(float deltaT, Kore::vec3 playerPosition)
{
	inAnimation = animator->executeAnimation(entity, animationLibrary.at("Punching"), logger);
	if (inAnimation) return AIState::Attacking;
	else return AIState::Planning;
}

CyborgAI::AIState CyborgAI::pursueing(float deltaT, Kore::vec3 playerPosition)
{
	return AIState();
}

CyborgAI::AIState CyborgAI::planning(float deltaT, Kore::vec3 playerPosition)
{
	return AIState();
}

CyborgAI::CyborgAI(AnimatedEntity* enemyEntity, Animator* animatorReference) : StateMachineAI(enemyEntity, animatorReference)
{
	stateToAction = {
		{(StateMachineAI::AIState) AIState::Attacking, reinterpret_cast<action>(&CyborgAI::attacking)},
		{(StateMachineAI::AIState) AIState::Pursueing, reinterpret_cast<action>(&CyborgAI::pursueing)},
		{(StateMachineAI::AIState) AIState::Planning, reinterpret_cast<action>(&CyborgAI::planning)}
	};
	animationLibrary = {
		{"Punching", "InsertFileName and path here"}
	};
}
