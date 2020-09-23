#include "StateMachineAI.h"

//Implementation of the abstract AI and it's statemachine functionality
void StateMachineAI::update(float deltaT)
{
	if (entity->activated) {
		currentState = (this->*stateToAction.at(currentState))(deltaT);
		continueMovement(deltaT);
		Kore::log(Kore::Info, "Ninja %s", entity->colorTag.c_str());
		Kore::log(Kore::Info, " Enemy Position is Y %f Z %f X %f", entity->position.y(), entity->position.z(),entity->position.x());
		Kore::log(Kore::Info, " posToMove Position is %f %f %f", posToMove.x(), posToMove.y(), posToMove.z());
		Kore::log(Kore::Info, " Enemy Position is %f %f %f", avatar->position.x(), avatar->position.y(), avatar->position.z());
	}
}

bool StateMachineAI::continueMovement(double deltaT) {

	bool changed = false;

	if (entity->position != toInvertPos(posToMove));
	{
		//Kore::log(Kore::Info, "X %f -%f ", entity->position.x(), posToMove.z());

		if ((unsigned)entity->position.y() - (unsigned)posToMove.x() > 0.1f)
		{
			if (entity->position.y() > -1 * posToMove.x())
				entity->position.y() -= deltaT * speed;
			else entity->position.y() += deltaT * speed;

			changed = true;
		}

		//Kore::log(Kore::Info, "Y %f -%f ", entity->position.y(), posToMove.x());

		if (entity->position.z() - posToMove.y() > 0.1f)
		{
			if (entity->position.z() > posToMove.y())
				entity->position.z() -= deltaT * speed * 6;
			else entity->position.z() += deltaT * speed * 6;
			changed = true;
		}
		//Kore::log(Kore::Info, "Z %f -%f ", entity->position.z(), posToMove.y());
		if ((unsigned)entity->position.x() - (unsigned)posToMove.z() > 0.1f)
		{
			if (entity->position.x() > -1 * posToMove.z())
				entity->position.x() -= deltaT * speed;
			else entity->position.x() += deltaT * speed;
			changed = true;
		}
	}
	return changed;
		//entity->position.x() = -1 * avatar->position.z();
		//entity->position.y() = -1 * avatar->position.x() + 2;
		//entity->position.z() = -1 * avatar->position.y();
		
}

Kore::vec3 StateMachineAI::toInvertPos(Kore::vec3 pos) { return Kore::vec3(-1 * pos.z(), -1 * pos.x(), pos.y()); }
Kore::vec3 StateMachineAI::toOriginPos(Kore::vec3 pos) {return Kore::vec3(-1 *pos.y(), pos.z(), -1 * pos.x());
}

void StateMachineAI::setEntityPos(Kore::vec3 pos) {	entity->position = toInvertPos(pos);
}

StateMachineAI::StateMachineAI(AnimatedEntity* enemyEntity, Animator* animatorReference, Avatar* avatarReference)
{
	entity = enemyEntity;
	animator = animatorReference;
	avatar = avatarReference;
	logger = new Logger();
	inAnimation = false;
	posToMove = Kore::vec3(0, 0, 0);
}

CyborgAI::AIState CyborgAI::falling(float deltaT)
{
	//if(continueMovement(0))
		inAnimation = animator->executeAnimation(entity, animationLibrary.at("Running"), logger);

	if (inAnimation || entity->position.z() > 0.1)
	{
		Kore::log(Kore::Info, "stay zu Falling %d", currentState);
		return AIState::Falling;
		lastState = currentState;
	}
	else
	{
		Kore::log(Kore::Info, "Jump zu Landing %d", currentState);
		entity->position.z() = 0;
		return AIState::Landing;
	}
}

CyborgAI::AIState CyborgAI::attacking(float deltaT)
{
	if (!inAnimation) {
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
		dead--;
	}

	if (inAnimation || dead > 0)
		inAnimation = animator->executeAnimation(entity, animationLibrary.at(currentAnimation), logger);

	if (avatar->movementExpired || (avatar->lastMovement != Avatar::LateralBounding)) avatar->hit(); //Example of movement recognition-implementation


	if (inAnimation)
	{
		Kore::log(Kore::Info, "Stay in Attacking %d", currentState);
		return AIState::Attacking;
	}
	else if (entity->isDead()|| dead <= 0)
	{
		Kore::log(Kore::Info, " Jump to Dying %d", currentState);
		return AIState::Dying;
	}
	Kore::log(Kore::Info, " Jump to Planning %d", currentState);
	return AIState::Planning;
}

//We calculate the direction vectors between the current enemy and other enemies but also between the current enemy and the avatar. 
//With these direction vectors we calculate the radians  based on tha scalar products. With the cross product we decide which direction the rotation has. 
CyborgAI::AIState CyborgAI::pursueing(float deltaT)
{

		inAnimation = animator->executeAnimation(entity, animationLibrary.at("Running"), logger);

	if (inAnimation)
	{
		Kore::log(Kore::Info, "stay in Pursuing %d", currentState);
		return AIState::Pursueing;
	}
	else if (entity->isDead())
	{
		Kore::log(Kore::Info, "jump to Dying %d", currentState);
		return AIState::Dying;
	}
	Kore::log(Kore::Info, " jump to Planning %d", currentState);
	return AIState::Planning;
}

CyborgAI::AIState CyborgAI::dying(float deltaT)
{
	(rand() % 2 == 1) ? currentAnimation = "Dying1" : currentAnimation = "Dying2";

	inAnimation = animator->executeAnimation(entity, animationLibrary.at(currentAnimation), logger);

	if (inAnimation)
	{
		Kore::log(Kore::Info, "stay in Dying %d", currentState);
		return AIState::Dying;
	}
	else
	{
		entity->activated = false;
	}
}

//in planning we decide what is the next state 
CyborgAI::AIState CyborgAI::planning(float deltaT)
{	
	if (entity->position.z() > 0.1) {
		posToMove = Kore::vec3(entity->position.y(), 0, entity->position.x());
		Kore::log(Kore::Info, "Jump zu Falling %d", currentState);
		return AIState::Falling;
	}

	if (toOriginPos(entity->position).x() - avatar->position.x() <= 2) {
		Kore::log(Kore::Info, "Jump zu Attacking %d", currentState);
		return AIState::Attacking;
	}

	if (toOriginPos(entity->position).x() - avatar->position.x() >= 2) {
		posToMove = Kore::vec3(avatar->position.x() + 2, entity->position.z(), avatar->position.z());
		Kore::log(Kore::Info, "Jump zu Pursueing %d", currentState);
		return AIState::Pursueing;
	}
}

CyborgAI::AIState CyborgAI::landing(float deltaT)
{
	inAnimation = animator->executeAnimation(entity, animationLibrary.at("Running"), logger);
	if (inAnimation) {
		Kore::log(Kore::Info, "stay zu Falling %d", currentState);
		return AIState::Landing;	
	}
	else {
		Kore::log(Kore::Info, "Jump zu Planning %d", currentState);
			return AIState::Planning; }
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
