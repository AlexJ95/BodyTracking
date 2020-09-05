#pragma once

#include "pch.h"

#include "Settings.h"
#include "InverseKinematics.h"
#include "MeshObject.h"

class AnimatedEntity
{
private:
	float currentHeight;
public:
	Kore::vec3 position;
	Kore::Quaternion rotation;
	EndEffector** endEffector;
	InverseKinematics* invKin;
	bool activated=false;
	bool beated = false;
	bool calibrated;

	float strength = 1.0;	//strength=1.0 -> mit einem Schlag ist di Entity tod
	MeshObject* meshObject;
	AnimatedEntity(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 initialPosition, Kore::Quaternion initialRotation);
	AnimatedEntity(MeshObject* meshReference, Kore::vec3 initialPosition, Kore::Quaternion initialRotation);
	void initializeEndeffectors();
	void hit();
	void resetCurrentHeight();
	bool isDead();
	bool attackedSuccessfully = false;

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
public:
	// Motion Recognition logic
	enum PlayerMovement {Jogging, Kick, KickPunch, LateralBounding, Lunges, Punch, Sitting, Squats, Standing, Walking};
	PlayerMovement lastMovement = Standing;
	bool movementExpired = true;
	float movementExpiration = 0;

	std::vector<MeshObject*> viveObjects;
	bool renderTrackerAndControllers;
	bool renderAxisForEndEffectors;

	Avatar(const char* meshFile, const char* textureFile, const Kore::Graphics4::VertexStructure& structure, float scale, Kore::vec3 position, Kore::Quaternion rotation, bool renderTrackerAndController, bool renderAxisForEndEffector);
	
	void update(float deltaT);

	void recognizedMotion(PlayerMovement currentMovement); //this will be called from the MotionRecognitionClass whenever it recognizes a new motion
};
