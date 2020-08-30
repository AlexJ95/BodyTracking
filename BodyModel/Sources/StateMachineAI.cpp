#include "StateMachineAI.h"

void StateMachineAI::update(float deltaT, Kore::vec3 playerPosition)
{
	if(entity->activated)
		currentState = (this->*stateToAction.at(currentState))(deltaT, playerPosition);
}

StateMachineAI::StateMachineAI(AnimatedEntity* enemyEntity, Animator* animatorReference)
{
	entity = enemyEntity;
	animator = animatorReference;
	logger = new Logger();
	inAnimation = false;
}

void StateMachineAI::spawn()
{
	Kore::vec3 startPos((float)(rand() % 10), (float)(rand() % 10),0.0f);
	entity->position = startPos;
	entity->activated = true;
	entity->currentHeight = 100.0;
}

void StateMachineAI::respawn()			//Vielleicht extra AIState mit Animation "Sterben"
{
	entity->activated = false;
}

void StateMachineAI::hit()				//vielleicht verschiedene Attacken einbauen
{
	entity->currentHeight -= 20.0 / entity->strength;
}

void StateMachineAI::checkColision(Kore::vec3 posOtherEnemy)
{
	float distanceBetweeenEnemys = (entity->position - posOtherEnemy).getLength();
	if (distanceBetweeenEnemys < maxDistanceToEnemy)
	{
		tooClose = true;
		currentPosOtherEnemy = posOtherEnemy;
	}

}

CyborgAI::AIState CyborgAI::attacking(float deltaT, Kore::vec3 playerPosition)
{
	inAnimation = animator->executeAnimation(entity, animationLibrary.at("Kicking"), logger);
	if (inAnimation) return AIState::Attacking;
	else return AIState::Planning;
}

CyborgAI::AIState CyborgAI::pursueing(float deltaT, Kore::vec3 playerPosition)
{
	Kore::vec3 dirBetweenEnemys = entity->position - currentPosOtherEnemy;
	Kore::vec3 dirBetweenEnemysGlob = locToGlob * Kore::vec4(dirBetweenEnemys.x(), dirBetweenEnemys.y(), dirBetweenEnemys.z(), 1.0);
	dirBetweenEnemysGlob.y() = 0;
	dirBetweenEnemysGlob.normalize();

	Kore::vec3 entityPosGlob = locToGlob * Kore::vec4(entity->position.x(), entity->position.y(), entity->position.z(), 1.0);
	Kore::vec3 entToPlayerDir = (playerPosition - entityPosGlob);
	entToPlayerDir.y()=0;
	float currentDistance = entToPlayerDir.getLength();
	entToPlayerDir.normalize();

	Kore::vec3 prevDir = locToGlob * (entity->rotation.matrix() * Kore::vec4(0, -1, 0, 1));
	prevDir.normalize();
	
	if (!tooClose  && Kore::abs(entToPlayerDir * prevDir) < 0.999)
	{
		float sign = entToPlayerDir.cross(prevDir).y();

		if (sign > 0)
			radians += dRot * Kore::acos(entToPlayerDir * prevDir);
		else
			radians -= dRot * Kore::acos(entToPlayerDir * prevDir);

		entity->rotation = Kore::Quaternion(Kore::vec3(0, 0, 1), radians);
		float a = Kore::abs(entToPlayerDir * prevDir);
	

	}
	else if (tooClose && dirBetweenEnemys.getLength() < maxDistanceToEnemy * 3.0 && Kore::abs(dirBetweenEnemysGlob * prevDir) < 0.999)
	{
		float sign = entToPlayerDir.cross(dirBetweenEnemysGlob).y();

		if (sign > 0)
			radians -= dRotCol * Kore::acos(dirBetweenEnemysGlob * prevDir);
		else
			radians += dRotCol * Kore::acos(dirBetweenEnemysGlob * prevDir);

		entity->rotation = Kore::Quaternion(Kore::vec3(0, 0, 1), radians);

	}
	else if(dirBetweenEnemys.getLength() > maxDistanceToEnemy * 3.0)
		tooClose = false;

	if (currentDistance > maxDistanceToPlayer | tooClose)
		entity->position += dTrans * (locToGlob.Invert() * Kore::vec4(prevDir.x(), prevDir.y(), prevDir.z(), 1.0));

	inAnimation = animator->executeAnimation(entity, animationLibrary.at("Walking"), logger);
	if (inAnimation) return AIState::Pursueing;
	else return AIState::Planning;
}

CyborgAI::AIState CyborgAI::planning(float deltaT, Kore::vec3 playerPosition)
{
	Kore::vec3 entityPosGlob = locToGlob * Kore::vec4(entity->position.x(), entity->position.y(), entity->position.z(), 1.0);
	Kore::vec3 entToPlayerDir = (playerPosition - entityPosGlob);
	Kore::vec3 prevDir = locToGlob * (entity->rotation.matrix() * Kore::vec4(0, -1, 0, 1));

	entToPlayerDir.y() = 0;
	prevDir.y() = 0;
	float currentDistance = entToPlayerDir.getLength();
	float crossValue = entToPlayerDir.cross(prevDir).getLength();

	if (entity->currentHeight <= 0.f)
	{
		respawn();
		return AIState::Planning;
	}
	else if (currentDistance <= maxDistanceToPlayer & crossValue <0.1) return AIState::Attacking;
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
