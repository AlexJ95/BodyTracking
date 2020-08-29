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
	inAnimation = animator->executeAnimation(entity, animationLibrary.at("Kicking"), logger);
	if (inAnimation) return AIState::Attacking;
	else return AIState::Planning;
}

CyborgAI::AIState CyborgAI::pursueing(float deltaT, Kore::vec3 playerPosition)
{
	Kore::vec3 entityPosGlob = locToGlob * Kore::vec4(entity->position.x(), entity->position.y(), entity->position.z(), 1.0);
	Kore::vec3 entToPlayerDir = (playerPosition - entityPosGlob);
	entToPlayerDir.y()=0;
	float currentDistance = entToPlayerDir.getLength();
	entToPlayerDir.normalize();

	Kore::vec3 prevDir = locToGlob * (entity->rotation.matrix() * Kore::vec4(0, -1, 0, 1));
	prevDir.normalize();
	float sign = entToPlayerDir.cross(prevDir).y();

	if (Kore::abs(entToPlayerDir * prevDir) < 0.999)
	{
		if (sign > 0)
			radians += dRot * Kore::acos(entToPlayerDir * prevDir);
		else
			radians -= dRot * Kore::acos(entToPlayerDir * prevDir);
	}

	entity->rotation = Kore::Quaternion(Kore::vec3(0,0,1), radians);

	if(currentDistance > maxDistance)
		entity->position += dTrans * (locToGlob.Invert() * Kore::vec4(prevDir.x(), prevDir.y(), prevDir.z(), 1.0));

	inAnimation = animator->executeAnimation(entity, animationLibrary.at("Walking"), logger);
	if (inAnimation) return AIState::Pursueing;
	else return AIState::Planning;
}

CyborgAI::AIState CyborgAI::planning(float deltaT, Kore::vec3 playerPosition)
{
	Kore::vec3 entityPosGlob = locToGlob * Kore::vec4(entity->position.x(), entity->position.y(), entity->position.z(), 1.0);
	Kore::vec3 entToPlayerDir = (playerPosition - entityPosGlob);
	entToPlayerDir.y() = 0;
	float currentDistance = entToPlayerDir.getLength();

	if (currentDistance <= maxDistance) return AIState::Attacking;
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
