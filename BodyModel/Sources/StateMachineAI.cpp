#include "StateMachineAI.h"

//Implementation of the abstract AI and it's statemachine functionality
void StateMachineAI::update(float deltaT)
{
	if(entity->activated)
		currentState = (this->*stateToAction.at(currentState))(deltaT);
}

StateMachineAI::StateMachineAI(AnimatedEntity* enemyEntity, Animator* animatorReference, Avatar* avatarReference)
{
	entity = enemyEntity;
	animator = animatorReference;
	avatar = avatarReference;
	logger = new Logger();
	inAnimation = false;
}

void StateMachineAI::spawn()
{
	//set the start height 
	entity->position.z() = 50;
	entity->activated = true;
	currentState = (StateMachineAI::AIState) CyborgAI::AIState::Falling;
}

//check for collision between the current enemy and the other enemies 
void StateMachineAI::checkCollision(Kore::vec3 posOtherEnemy)
{
	float distanceBetweeenEnemys = (entity->position - posOtherEnemy).getLength();
	if (distanceBetweeenEnemys < maxDistanceToEnemy)
	{
		tooClose = true;
		currentPosOtherEnemy = posOtherEnemy;
	}
}


int StateMachineAI::beatedEnemyCount = 0;
float StateMachineAI::lastDeadPos = 0.0;
int CyborgAI::numberOfVictories = 0;

CyborgAI::AIState CyborgAI::falling(float deltaT)
{
	inAnimation = animator->executeAnimation(entity, animationLibrary.at("Falling"), logger);

	entity->position.z() -= fallingVelocity * deltaT;
	fallingVelocity += fallingAcceleration * deltaT;

	//if the current enemy lands at the train, it begins the pathfinding
	
	if (entity->position.z() > 0)
	{
		return AIState::Falling;
	}
	else
	{
		entity->position.z() = 0.f;
		inAnimation = false;
		return AIState::Landing;
	}
}

CyborgAI::AIState CyborgAI::attacking(float deltaT)
{
	if (!inAnimation) 
		switch (rand() % 3) {
		case 0:
			currentAnimation = "VerticalChop"; break;
		case 1:
			currentAnimation = "HorizontalSweep"; break;
		case 2:
			currentAnimation = "Kicking"; break;
		default:
			currentAnimation = "VerticalChop"; break;
		}

	inAnimation = animator->executeAnimation(entity, animationLibrary.at(currentAnimation), logger);

	if (avatar->movementExpired || (avatar->lastMovement != Avatar::LateralBounding)) avatar->hit(); //Example of movement recognition-implementation


	if (inAnimation && !entity->isDead())
	{
		return AIState::Attacking;
	}
	else if (entity->isDead())
	{
		inAnimation = false;
		return AIState::Dying;
	}

	return AIState::Planning;
}

//We calculate the direction vectors between the current enemy and other enemies but also between the current enemy and the avatar. 
//With these direction vectors we calculate the radians  based on tha scalar products. With the cross product we decide which direction the rotation has. 
CyborgAI::AIState CyborgAI::pursueing(float deltaT)
{
	Kore::vec3 avatarPosGlob = locToGlob * Kore::vec4(avatar->position.x(), avatar->position.y(), avatar->position.z(), 1.0);
	avatarPosGlob.y() = 0;
	Kore::vec3 dirBetweenEnemys = entity->position - currentPosOtherEnemy;
	Kore::vec3 dirBetweenEnemysGlob = locToGlob * Kore::vec4(dirBetweenEnemys.x(), dirBetweenEnemys.y(), dirBetweenEnemys.z(), 1.0);
	dirBetweenEnemysGlob.y() = 0;
	dirBetweenEnemysGlob.normalize();
	

	Kore::vec3 entityPosGlob = locToGlob * Kore::vec4(entity->position.x(), entity->position.y(), entity->position.z(), 1.0);
	entityPosGlob.y() = 0;
	Kore::vec3 entToPlayerDir = (avatarPosGlob - entityPosGlob);
	float currentDistance = entToPlayerDir.getLength();
	entToPlayerDir.normalize();

	Kore::vec3 prevDir = locToGlob * (entity->rotation.matrix() * Kore::vec4(0, -1, 0, 1));
	prevDir.y() = 0;
	prevDir.normalize();

	if (!tooClose  && Kore::abs(entToPlayerDir * prevDir) < 0.999)
	{
		float sign = entToPlayerDir.cross(prevDir).y();


		if (sign > 0)
			radians += dRot * Kore::acos(entToPlayerDir * prevDir);
		else
			radians -= dRot * Kore::acos(entToPlayerDir * prevDir);


		entity->rotation = Kore::Quaternion(Kore::vec3(0, 0, 1), radians);
	}
	else if (tooClose && dirBetweenEnemys.getLength() < maxDistanceToEnemy * 3.5 && Kore::abs(dirBetweenEnemysGlob * prevDir) < 0.999)
	{
		float sign = entToPlayerDir.cross(dirBetweenEnemysGlob).y();

		if (sign > 0)
			radians -= dRotCol * Kore::acos(dirBetweenEnemysGlob * prevDir);
		else
			radians += dRotCol * Kore::acos(dirBetweenEnemysGlob * prevDir);
		

		entity->rotation = Kore::Quaternion(Kore::vec3(0, 0, 1), radians);
	}
	else if(dirBetweenEnemys.getLength() > maxDistanceToEnemy * 1.5)
		tooClose = false;

	if (currentDistance > maxDistanceToPlayer || tooClose)
	{
		
		entity->position += dTrans * (locToGlob.Invert() * Kore::vec4(prevDir.x(), prevDir.y(), prevDir.z(), 1.0));

		if (entity->position.x() > limitPosX)
		{
			entity->position.x() = limitPosX;
		}
		else if (entity->position.x() < -limitPosX)
		{
			entity->position.x() = -limitPosX;
		}
	}

	inAnimation = animator->executeAnimation(entity, animationLibrary.at("Running"), logger);

	if (inAnimation & !entity->isDead())
	{
		return AIState::Pursueing;
	}
	else if (entity->isDead())
	{
		inAnimation = false;
		return AIState::Dying;
	}
	return AIState::Planning;
}

CyborgAI::AIState CyborgAI::dying(float deltaT)
{
	lastDeadPos = entity->position.y();
	if (!inAnimation) currentAnimation = rand() % 2 == 1? "Dying1" : "Dying2";
	inAnimation = animator->executeAnimation(entity, animationLibrary.at(currentAnimation), logger);

	if (inAnimation)
	{
		return AIState::Dying;
	}
	else
	{
		entity->activated = false;
		entity->beated = true;
		beatedEnemyCount++;
		numberOfVictories++;
	}
	return AIState::Planning;
}

//in planning we decide what is the next state 
CyborgAI::AIState CyborgAI::planning(float deltaT)
{
	Kore::vec3 entityPosGlob = locToGlob * Kore::vec4(entity->position.x(), entity->position.y(), entity->position.z(), 1.0);
	Kore::vec3 avatarPosGlob = locToGlob * Kore::vec4(avatar->position.x(), avatar->position.y(), avatar->position.z(), 1.0);
	Kore::vec3 entToPlayerDir = (avatarPosGlob - entityPosGlob);
	Kore::vec3 prevDir = locToGlob * (entity->rotation.matrix() * Kore::vec4(0, -1, 0, 1));

	entToPlayerDir.y() = 0;
	prevDir.y() = 0;
	float currentDistance = entToPlayerDir.getLength();
	float crossValue = entToPlayerDir.cross(prevDir).getLength();

	if (currentDistance <= maxDistanceToPlayer & crossValue < 0.1)
	{
		return AIState::Attacking;
	}
	else
	{
		return AIState::Pursueing;
	}
	return AIState::Planning;
}

CyborgAI::AIState CyborgAI::landing(float deltaT)
{
	inAnimation = animator->executeAnimation(entity, animationLibrary.at("Landing"), logger);
	if (inAnimation) return AIState::Landing;
	return AIState::Planning;
}

CyborgAI::CyborgAI(AnimatedEntity* enemyEntity, Animator* animatorReference, Avatar* avatarReference) : StateMachineAI(enemyEntity, animatorReference, avatarReference)
{
	stateToAction =
	{
		{(StateMachineAI::AIState) CyborgAI::AIState::Attacking,	reinterpret_cast<action>(&CyborgAI::attacking)},
		{(StateMachineAI::AIState) CyborgAI::AIState::Pursueing,	reinterpret_cast<action>(&CyborgAI::pursueing)},
		{(StateMachineAI::AIState) CyborgAI::AIState::Planning,		reinterpret_cast<action>(&CyborgAI::planning)},
		{(StateMachineAI::AIState) CyborgAI::AIState::Landing,		reinterpret_cast<action>(&CyborgAI::landing)},
		{(StateMachineAI::AIState) CyborgAI::AIState::Dying,		reinterpret_cast<action>(&CyborgAI::dying)},
		{(StateMachineAI::AIState) CyborgAI::AIState::Falling,		reinterpret_cast<action>(&CyborgAI::falling)}
		
	};
	animationLibrary =
	{
		{"VerticalChop",	files[7]},
		{"Kicking",			files[8]},
		{"HorizontalSweep",	files[9]},
		{"Running",			files[14]},
		{"Dying1",			files[12]},
		{"Dying2",			files[13]},
		{"Landing",			files[11]},
		{"Falling",			files[10]}
	};
	currentState = (StateMachineAI::AIState) CyborgAI::AIState::Planning;
}
