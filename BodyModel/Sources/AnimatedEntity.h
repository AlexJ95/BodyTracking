#pragma once

#include "pch.h"

#include "Settings.h"
#include "InverseKinematics.h"
#include "MeshObject.h"

class AnimatedEntity
{
private:
	//currentHeight is the current life energy of the enemies
	float currentHeight;
public:
	Kore::vec3 position;
	Kore::Quaternion rotation;
	EndEffector** endEffector;
	InverseKinematics* invKin;
	bool activated = false;
	bool beated = false;
	bool calibrated;

	//strength controls the highest value substracted of currentHeight by calling hit()
	float strength = 1.0;	
	MeshObject* meshObject;
	AnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 initialPosition, Kore::Quaternion initialRotation);
	AnimatedEntity(MeshObject* meshReference, Kore::vec3 initialPosition, Kore::Quaternion initialRotation);
	void initializeEndeffectors();
	void hit();
	void resetCurrentHeight();
	bool isDead();
	bool attackedSuccessfully = false;
	//colorTag contains the string with the color of the attacked enemy, this is needed for showing the next attack in the gui on the top
	string colorTag;

	float getReached() const;
	float getStucked() const;
	float* getIterations() const;
	float* getErrorPos() const;
	float* getErrorRot() const;
	float* getTime() const;
	float* getTimeIteration() const;
};

class Avatar : public AnimatedEntity
{
	static Avatar* instance;
	

public:
	static Avatar* getInstance();
	// Motion Recognition logic
	enum PlayerMovement {Jogging, Kick, KickPunch, LateralBounding, Lunges, Punch, Sitting, Squats, Standing, Walking};
	PlayerMovement lastMovement = Standing;
	bool movementExpired = true;
	float movementExpiration = 0;

	bool moveForward = false;

	std::vector<MeshObject*> viveObjects;
	bool renderTrackerAndControllers;
	bool renderAxisForEndEffectors;

	Avatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation, bool renderTrackerAndController, bool renderAxisForEndEffector);
	
	void update(float deltaT);

	void triggerMovement();

	void recognizedMotion(PlayerMovement currentMovement); //this will be called from the MotionRecognitionClass whenever it recognizes a new motion
};
