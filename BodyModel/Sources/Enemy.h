#pragma once

#include "AnimatedEntity.h"
#include "Renderer.h"

class Enemy : public AnimatedEntity
{
protected:
	//the different states of the AI state machine
	enum AIStates {Attacking, Approaching, Defending, Planning};
	AIStates currentState;
	int health;
	
	float scaleFactor;
	
	void attack();
	void defend();
	void approach();
	void plan();

	void executeMovement(int endEffectorID);

public:
	//act is supposed to emulate the AI and will should be called from update(),
	//params will change according to the needs of the decisionmaking
	//act shouldn't carry the logic of the state machine which represents the decision making of the AI
	//act simply calls the method representing the current state of the state machine
	void act();
};
