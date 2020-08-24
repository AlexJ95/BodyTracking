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
	entity->rotation = Kore::Quaternion(entity->position - playerPosition, 0);
	inAnimation = animator->executeAnimation(entity, animationLibrary.at("Kicking"), logger);
	if (inAnimation) return AIState::Attacking;
	else return AIState::Planning;
}

CyborgAI::AIState CyborgAI::pursueing(float deltaT, Kore::vec3 playerPosition)
{
	entity->rotation = Kore::Quaternion(entity->position - playerPosition, 0);
	inAnimation = animator->executeAnimation(entity, animationLibrary.at("Walking"), logger);
	if (inAnimation) return AIState::Pursueing;
	else return AIState::Planning;
}

CyborgAI::AIState CyborgAI::planning(float deltaT, Kore::vec3 playerPosition)
{
	float f = (entity->position - playerPosition).getLength();
	if ((entity->position - playerPosition).getLength() < 2.0f) return AIState::Attacking;
	return AIState::Pursueing;
}

CyborgAI::CyborgAI(AnimatedEntity* enemyEntity, Animator* animatorReference) : StateMachineAI(enemyEntity, animatorReference)
{
	stateToAction = {
		{(StateMachineAI::AIState) CyborgAI::AIState::Attacking, reinterpret_cast<action>(&CyborgAI::attacking)},
		{(StateMachineAI::AIState) CyborgAI::AIState::Pursueing, reinterpret_cast<action>(&CyborgAI::pursueing)},
		{(StateMachineAI::AIState) CyborgAI::AIState::Planning, reinterpret_cast<action>(&CyborgAI::planning)}
	};
	animationLibrary = {
		{"Kicking", files[3]},
		{"Walking", files[1]}
	};
	currentState = (StateMachineAI::AIState) CyborgAI::AIState::Planning;
}
