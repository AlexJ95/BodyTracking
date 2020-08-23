#include "StateMachineAI.h"

StateMachineAI::StateMachineAI(AnimatedEntity* enemyEntity, Animator* animatorReference)
{
	entity = enemyEntity;
	animator = animatorReference;
	inAnimation = false;
}

void StateMachineAI::update(float deltaT)
{
}

CyborgAI::AIState CyborgAI::attacking(float deltaT, Kore::vec3 playerPosition)
{
	return AIState::Attacking;
}

CyborgAI::CyborgAI(AnimatedEntity* enemyEntity, Animator* animatorReference) : StateMachineAI(enemyEntity, animatorReference)
{
	/*stateToAction = {
		{AIState::Attacking, reinterpret_cast<action>((StateMachineAI::AIState*) &CyborgAI::attacking)}/*,
		{AIState::Pursueing, this->pursueing()},
		{AIState::Planning, this->planning()}*/
	//};
}
