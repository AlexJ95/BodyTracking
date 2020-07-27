#include "pch.h"
#include <iostream>

#include <Kore/IO/FileReader.h>
#include <Kore/Graphics4/PipelineState.h>
#include <Kore/Graphics1/Color.h>
#include <Kore/Input/Keyboard.h>
#include <Kore/Input/Mouse.h>
#include <Kore/Audio2/Audio.h>
#include <Kore/Audio1/Audio.h>
#include <Kore/Audio1/Sound.h>
#include <Kore/Audio1/SoundStream.h>
#include <Kore/System.h>
#include <Kore/Log.h>

#include "Settings.h"
#include "EndEffector.h"
#include "Avatar.h"
#include "LivingRoom.h"
#include "Logger.h"

#include <algorithm> // std::sort

#ifdef KORE_STEAMVR
#include <Kore/Vr/VrInterface.h>
#include <Kore/Vr/SensorState.h>
#include <Kore/Input/Gamepad.h>
#endif

using namespace Kore;
using namespace Kore::Graphics4;
using namespace std;

namespace {
	const int width = 1024;
	const int height = 768;
	
	const bool renderRoom = true;
	const bool renderTrackerAndController = true;
	const bool renderAxisForEndEffector = false;
	
	EndEffector** endEffector;
	const int numOfEndEffectors = 8;
	
	Logger* logger;
	
	double startTime;
	double lastTime;
	float cameraMoveSpeed = 10.f;
	int trainSpeed = 100;
	
	// Audio cues
	Sound* startRecordingSound;
	Sound* stopRecordingSound;
	
	// Avatar shader
	VertexStructure structure;
	Shader* vertexShader;
	Shader* fragmentShader;
	PipelineState* pipeline;
	
	TextureUnit tex;
	ConstantLocation pLocation;
	ConstantLocation vLocation;
	ConstantLocation mLocation;
	Kore::Quaternion livingRoomRot;
	
	// Living room shader
	VertexStructure structure_living_room;
	Shader* vertexShader_living_room;
	Shader* fragmentShader_living_room;
	PipelineState* pipeline_living_room;
	
	TextureUnit tex_living_room;
	ConstantLocation pLocation_living_room;
	ConstantLocation vLocation_living_room;
	ConstantLocation mLocation_living_room;
	ConstantLocation mLocation_living_room_inverse;
	ConstantLocation diffuse_living_room;
	ConstantLocation specular_living_room;
	ConstantLocation specular_power_living_room;
	ConstantLocation lightPosLocation_living_room;
	ConstantLocation lightCount_living_room;
	
	// Keyboard controls
	bool rotate = false;
	bool W, A, S, D = false;
	
	vec4 camUp(0.0f, 1.0f, 0.0f, 0.0f);
	vec4 camForward(0.0f, 0.0f, 1.0f, 0.0f);
	vec4 camRight(1.0f, 0.0f, 0.0f, 0.0f);
	
	vec3 cameraPos(0, 0, 0);
	
	// Null terminated array of MeshObject pointers (Vive Controller and Tracker)
	MeshObject* viveObjects[] = { nullptr, nullptr, nullptr };
	//3D Objects
	Avatar* avatar;
	LivingRoom* train;
	LivingRoom* floor;
	//List to track the position of all floortiles
	mat4 floorCoords[80];
	//Offsets needed to make the tiles loop seamlessly
	int offsetZfloor = 26;
	float offsetXfloor = -0.175f;
	//House objects
	LivingRoom* houseSmall;
	LivingRoom* houseMiddle;
	LivingRoom* houseML;
	LivingRoom* houseLarge;
	//Lists to track the position of all house objects on the left and on the right
	mat4 houseCoordsL[110];
	mat4 houseCoordsL2[110];
	mat4 houseCoordsL3[110];
	mat4 houseCoordsR[110];
	mat4 houseCoordsR2[110];
	mat4 houseCoordsR3[110];
	//Lists to track the kind house a certain object is supposed to be
	//Contains 0,1,2 or 3 for: SmallHouse, MiddleHouse, MLHouse and LargeHouse
	int houseElementListL[110];
	int houseElementListL2[110];
	int houseElementListL3[110];
	int houseElementListR[110];
	int houseElementListR2[110];
	int houseElementListR3[110];
	//Offsets for the house loops
	int offsetZhouse = 16;
	float offsetXhouse = -0.115f;

	LivingRoom* helicopter;

	LivingRoom* skybox;
	
	// Variables to mirror the room and the avatar
	vec3 mirrorOver(6.057f, 0.0f, 0.04f);
	
	mat4 initTrans;
	mat4 initTransInv;
	Kore::Quaternion initRot;
	Kore::Quaternion initRotInv;
	
	bool calibratedAvatar = false;
	
#ifdef KORE_STEAMVR
	bool controllerButtonsInitialized = false;
	float currentUserHeight;
	bool firstPersonMonitor = false;
#else
	int loop = 0;
#endif
	
	void renderVRDevice(int index, Kore::mat4 M) {
		Graphics4::setMatrix(mLocation, M);
		viveObjects[index]->render(tex);
	}
	
	mat4 getMirrorMatrix() {
		Kore::Quaternion rot(0, 0, 0, 1);
		rot.rotate(Kore::Quaternion(vec3(0, 1, 0), Kore::pi));
		mat4 zMirror = mat4::Identity();
		zMirror.Set(2, 2 , -1);
		Kore::mat4 M = zMirror * mat4::Translation(mirrorOver.x(), mirrorOver.y(), mirrorOver.z()) * rot.matrix().Transpose();
		
		return M;
	}
	
	void renderControllerAndTracker(int tracker, Kore::vec3 desPosition, Kore::Quaternion desRotation) {
		// World Transformation Matrix
		Kore::mat4 W = mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z()) * desRotation.matrix().Transpose();
		
		// Mirror Transformation Matrix
		Kore::mat4 M = getMirrorMatrix() * W;
		
		if (tracker) {
			// Render a tracker for both feet and back
			renderVRDevice(0, W);
			//renderVRDevice(0, M);
		} else {
			// Render a controller for both hands
			renderVRDevice(1, W);
			//renderVRDevice(1, M);
		}
		
		// Render a local coordinate system only if the avatar is not calibrated
		if (!calibratedAvatar) {
			renderVRDevice(2, W);
			//renderVRDevice(2, M);
		}
	}
	
	void renderAllVRDevices() {
		Graphics4::setPipeline(pipeline);
	
#ifdef KORE_STEAMVR
		VrPoseState controller;
		for (int i = 0; i < 16; ++i) {
			controller = VrInterface::getController(i);
			
			vec3 desPosition = controller.vrPose.position;
			Kore::Quaternion desRotation = controller.vrPose.orientation;
			
			if (controller.trackedDevice == TrackedDevice::ViveTracker) {
				renderControllerAndTracker(true, desPosition, desRotation);
			} else if (controller.trackedDevice == TrackedDevice::Controller) {
				renderControllerAndTracker(false, desPosition, desRotation);
			}
			
		}
#else
		for(int i = 0; i < numOfEndEffectors; ++i) {
			Kore::vec3 desPosition = endEffector[i]->getDesPosition();
			Kore::Quaternion desRotation = endEffector[i]->getDesRotation();
			
			if (i == hip || i == leftForeArm || i == rightForeArm || i == leftFoot || i == rightFoot) {
				renderControllerAndTracker(true, desPosition, desRotation);
			} else if (i == rightHand || i == leftHand) {
				renderControllerAndTracker(false, desPosition, desRotation);
			}
		}
#endif
	}
	
	void renderCSForEndEffector() {
		Graphics4::setPipeline(pipeline);
		
		for(int i = 0; i < numOfEndEffectors; ++i) {
			BoneNode* bone = avatar->getBoneWithIndex(endEffector[i]->getBoneIndex());
			
			vec3 endEffectorPos = bone->getPosition();
			endEffectorPos = initTrans * vec4(endEffectorPos.x(), endEffectorPos.y(), endEffectorPos.z(), 1);
			Kore::Quaternion endEffectorRot = initRot.rotated(bone->getOrientation());
			
			Kore::mat4 M = mat4::Translation(endEffectorPos.x(), endEffectorPos.y(), endEffectorPos.z()) * endEffectorRot.matrix().Transpose();
			Graphics4::setMatrix(mLocation, M);
			viveObjects[2]->render(tex);
		}
	}
	
	void renderLivingRoom(mat4 V, mat4 P, float deltaTime) {
		Graphics4::setPipeline(pipeline_living_room);
		
		Graphics4::setMatrix(vLocation_living_room, V);
		Graphics4::setMatrix(pLocation_living_room, P);
		train->setLights(lightCount_living_room, lightPosLocation_living_room);
		train->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
		
		//Render the floor consisting of 80 tiles aligned seamlessly
		for (int x = 0; x < 80; x++) {
			//Push all tiles forward
			floorCoords[x] = floorCoords[x] * mat4::Translation(offsetZfloor * 0.05 * deltaTime * trainSpeed, offsetXfloor * 0.05 * deltaTime * trainSpeed, 0);
			//If the tile is at the "end" port it to the start
			if (floorCoords[x].data[14] <= -819) {
				floorCoords[x] = mat4::Translation(5.5 + offsetXfloor * 79, -3.75, -819 + offsetZfloor * 79) * livingRoomRot.matrix().Transpose();
			}
			floor->M = floorCoords[x];
			floor->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
		}

		//Render the houses/objects consisting of 110 houses on each side
		for (int x = 0; x < 110; x++) {
			//Push all houses/objects forward
			houseCoordsL[x] = houseCoordsL[x] * mat4::Translation(offsetXfloor * 0.15 * deltaTime * trainSpeed, -offsetZfloor * 0.05 * deltaTime * trainSpeed, 0);
			//houseCoordsL[x] = houseCoordsL[x] * mat4::Translation(offsetXfloor * deltaTime * trainSpeed, -offsetZfloor * deltaTime * trainSpeed, 0);
			houseCoordsL2[x] = houseCoordsL2[x] * mat4::Translation(offsetXfloor * 0.15, -offsetZfloor * 0.05, 0);
			houseCoordsL3[x] = houseCoordsL3[x] * mat4::Translation(offsetXfloor * 0.15, -offsetZfloor * 0.05, 0);
			houseCoordsR[x] = houseCoordsR[x] * mat4::Translation(offsetXfloor * 0.15 * deltaTime * trainSpeed, -offsetZfloor * 0.05 * deltaTime * trainSpeed, 0);
			//houseCoordsR[x] = houseCoordsR[x] * mat4::Translation(offsetXfloor * deltaTime * trainSpeed, -offsetZfloor * deltaTime * trainSpeed, 0);
			houseCoordsR2[x] = houseCoordsR2[x] * mat4::Translation(offsetXfloor * 0.15, -offsetZfloor * 0.05, 0);
			houseCoordsR3[x] = houseCoordsR3[x] * mat4::Translation(offsetXfloor * 0.15, -offsetZfloor * 0.05, 0);
			//If the object is at the "end" port it to the start and reassign a random kind of house to the object
			if (houseCoordsL[x].data[14] <= -819) {
				houseCoordsL[x] = mat4::Translation(22 + offsetXhouse * 109, -2.9, -819 + offsetZhouse * 109) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
				houseElementListL[x] = rand() % 4;
			}
			if (houseCoordsL2[x].data[14] <= -810) {
				houseCoordsL2[x] = mat4::Translation(34 + offsetXhouse * 109, -2.9, -810 + offsetZhouse * 109) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
				houseElementListL2[x] = rand() % 4;
			}
			if (houseCoordsL2[x].data[14] <= -823) {
				houseCoordsL2[x] = mat4::Translation(46 + offsetXhouse * 109, -2.9, -823 + offsetZhouse * 109) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
				houseElementListL3[x] = rand() % 4;
			}
			if (houseCoordsR[x].data[14] <= -819) {
				houseCoordsR[x] = mat4::Translation(-9 + offsetXhouse * 109, -2.9, -819 + offsetZhouse * 109) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
				houseElementListR[x] = rand() % 4;
			}
			if (houseCoordsR2[x].data[14] <= -813) {
				houseCoordsR2[x] = mat4::Translation(-21 + offsetXhouse * 109, -2.9, -813 + offsetZhouse * 109) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
				houseElementListR2[x] = rand() % 4;
			}
			if (houseCoordsR3[x].data[14] <= -825) {
				houseCoordsR3[x] = mat4::Translation(-33 + offsetXhouse * 109, -2.9, -825 + offsetZhouse * 109) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
				houseElementListR3[x] = rand() % 4;
			}

			switch (houseElementListL[x]) {
			case 0:
				houseSmall->M = houseCoordsL[x];
				houseSmall->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 1:
				houseMiddle->M = houseCoordsL[x];
				houseMiddle->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 2:
				houseML->M = houseCoordsL[x];
				houseML->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 3:
				houseLarge->M = houseCoordsL[x];
				houseLarge->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			}

			switch (houseElementListL2[x]) {
			case 0:
				houseSmall->M = houseCoordsL2[x];
				houseSmall->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 1:
				houseMiddle->M = houseCoordsL2[x];
				houseMiddle->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 2:
				houseML->M = houseCoordsL2[x];
				houseML->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 3:
				houseLarge->M = houseCoordsL2[x];
				houseLarge->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			}

			switch (houseElementListL3[x]) {
			case 0:
				houseSmall->M = houseCoordsL3[x];
				houseSmall->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 1:
				houseMiddle->M = houseCoordsL3[x];
				houseMiddle->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 2:
				houseML->M = houseCoordsL3[x];
				houseML->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 3:
				houseLarge->M = houseCoordsL3[x];
				houseLarge->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			}

			switch (houseElementListR[x]) {
			case 0:
				houseSmall->M = houseCoordsR[x];
				houseSmall->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 1:
				houseMiddle->M = houseCoordsR[x];
				houseMiddle->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 2:
				houseML->M = houseCoordsR[x];
				houseML->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 3:
				houseLarge->M = houseCoordsR[x];
				houseLarge->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			}

			switch (houseElementListR2[x]) {
			case 0:
				houseSmall->M = houseCoordsR2[x];
				houseSmall->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 1:
				houseMiddle->M = houseCoordsR2[x];
				houseMiddle->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 2:
				houseML->M = houseCoordsR2[x];
				houseML->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 3:
				houseLarge->M = houseCoordsR2[x];
				houseLarge->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			}

			switch (houseElementListR3[x]) {
			case 0:
				houseSmall->M = houseCoordsR3[x];
				houseSmall->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 1:
				houseMiddle->M = houseCoordsR3[x];
				houseMiddle->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 2:
				houseML->M = houseCoordsR3[x];
				houseML->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			case 3:
				houseLarge->M = houseCoordsR3[x];
				houseLarge->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
				break;
			}

		}
		
		skybox->render(tex_living_room, mLocation_living_room, mLocation_living_room_inverse, diffuse_living_room, specular_living_room, specular_power_living_room, false);
		
	}
	
	void renderAvatar(mat4 V, mat4 P) {
		Graphics4::setPipeline(pipeline);
		
		Graphics4::setMatrix(vLocation, V);
		Graphics4::setMatrix(pLocation, P);
		Graphics4::setMatrix(mLocation, initTrans);
		avatar->animate(tex);
		
		// Mirror the avatar
		//mat4 initTransMirror = getMirrorMatrix() * initTrans;
		
		//Graphics4::setMatrix(mLocation, initTransMirror);
		//avatar->animate(tex);
		
	}
	
	Kore::mat4 getProjectionMatrix() {
		mat4 P = mat4::Perspective(45, (float)width / (float)height, 0.01f, 1000);
		P.Set(0, 0, -P.get(0, 0));
		
		return P;
	}
	
	Kore::mat4 getViewMatrix() {
		mat4 V = mat4::lookAlong(camForward.xyz(), cameraPos, vec3(0.0f, 1.0f, 0.0f));
		return V;
	}
	
	void executeMovement(int endEffectorID) {
		Kore::vec3 desPosition = endEffector[endEffectorID]->getDesPosition();
		Kore::Quaternion desRotation = endEffector[endEffectorID]->getDesRotation();

		// Save raw data
		if (logRawData) logger->saveData(endEffector[endEffectorID]->getName(), desPosition, desRotation, avatar->scale);
		
		if (calibratedAvatar) {
			// Transform desired position/rotation to the character local coordinate system
			desPosition = initTransInv * vec4(desPosition.x(), desPosition.y(), desPosition.z(), 1);
			desRotation = initRotInv.rotated(desRotation);

			// Add offset
			Kore::Quaternion offsetRotation = endEffector[endEffectorID]->getOffsetRotation();
			vec3 offsetPosition = endEffector[endEffectorID]->getOffsetPosition();
			Kore::Quaternion finalRot = desRotation.rotated(offsetRotation);
			vec3 finalPos = mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z()) * finalRot.matrix().Transpose() * mat4::Translation(offsetPosition.x(), offsetPosition.y(), offsetPosition.z()) * vec4(0, 0, 0, 1);
			
			if (endEffectorID == hip) {
				avatar->setFixedPositionAndOrientation(endEffector[endEffectorID]->getBoneIndex(), finalPos, finalRot);
			} else if (endEffectorID == head || endEffectorID == leftForeArm || endEffectorID == rightForeArm || endEffectorID == leftFoot || endEffectorID == rightFoot) {
				avatar->setDesiredPositionAndOrientation(endEffector[endEffectorID]->getBoneIndex(), endEffector[endEffectorID]->getIKMode(), finalPos, finalRot);
			} else if (endEffectorID == leftHand || endEffectorID == rightHand) {
				avatar->setFixedOrientation(endEffector[endEffectorID]->getBoneIndex(), finalRot);
			}
		}
	}
	
	void initTransAndRot() {
		initRot = Kore::Quaternion(0, 0, 0, 1);
		initRot.rotate(Kore::Quaternion(vec3(1, 0, 0), -Kore::pi / 2.0));
		initRot.rotate(Kore::Quaternion(vec3(0, 0, 1), Kore::pi / 2.0));
		initRot.normalize();
		initRotInv = initRot.invert();
		
		vec3 initPos = vec4(0, 0, 0, 1);
		initTrans = mat4::Translation(initPos.x(), initPos.y(), initPos.z()) * initRot.matrix().Transpose();
		initTransInv = initTrans.Invert();
	}
	
	void calibrate() {
		initTransAndRot();
		
		for (int i = 0; i < numOfEndEffectors; ++i) {
			Kore::vec3 desPosition = endEffector[i]->getDesPosition();
			Kore::Quaternion desRotation = endEffector[i]->getDesRotation();
			
			// Transform desired position/rotation to the character local coordinate system
			desPosition = initTransInv * vec4(desPosition.x(), desPosition.y(), desPosition.z(), 1);
			desRotation = initRotInv.rotated(desRotation);
			
			// Get actual position/rotation of the character skeleton
			BoneNode* bone = avatar->getBoneWithIndex(endEffector[i]->getBoneIndex());
			vec3 targetPos = bone->getPosition();
			Kore::Quaternion targetRot = bone->getOrientation();
			
			endEffector[i]->setOffsetPosition((mat4::Translation(desPosition.x(), desPosition.y(), desPosition.z()) * targetRot.matrix().Transpose()).Invert() * mat4::Translation(targetPos.x(), targetPos.y(), targetPos.z()) * vec4(0, 0, 0, 1));
			endEffector[i]->setOffsetRotation((desRotation.invert()).rotated(targetRot));
		}
	}
	
	void record() {
		logRawData = !logRawData;
		
		if (logRawData) {
			Audio1::play(startRecordingSound);
			logger->startLogger("logData");
		} else {
			Audio1::play(stopRecordingSound);
			logger->endLogger();
		}
	}

#ifdef KORE_STEAMVR
	void setSize() {
		float currentAvatarHeight = avatar->getHeight();
		
		SensorState state = VrInterface::getSensorState(0);
		vec3 hmdPos = state.pose.vrPose.position; // z -> face, y -> up down
		currentUserHeight = hmdPos.y();
		
		float scale = currentUserHeight / currentAvatarHeight;
		avatar->setScale(scale);
		
		log(Info, "current avatar height %f, current user height %f ==> scale %f", currentAvatarHeight, currentUserHeight, scale);
	}
	
	void initEndEffector(int efID, int deviceID, vec3 pos, Kore::Quaternion rot) {
		endEffector[efID]->setDeviceIndex(deviceID);
		endEffector[efID]->setDesPosition(pos);
		endEffector[efID]->setDesRotation(rot);
		
		log(Info, "%s, device id: %i", endEffector[efID]->getName(), deviceID);
	}
	
	void assignControllerAndTracker() {
		VrPoseState vrDevice;
		
		const int numTrackers = 5;
		int trackerCount = 0;
		
		std::vector<EndEffector*> trackers;
		
		// Get indices for VR devices
		for (int i = 0; i < 16; ++i) {
			vrDevice = VrInterface::getController(i);
			
			vec3 devicePos = vrDevice.vrPose.position;
			Kore::Quaternion deviceRot = vrDevice.vrPose.orientation;

			if (vrDevice.trackedDevice == TrackedDevice::ViveTracker) {
				EndEffector* tracker = new EndEffector(-1);
				tracker->setDeviceIndex(i);
				tracker->setDesPosition(devicePos);
				tracker->setDesRotation(deviceRot);
				trackers.push_back(tracker);
				
				++trackerCount;
				if (trackerCount == numTrackers) {
					// Sort trackers regarding the y-Axis (height)
					std::sort(trackers.begin(), trackers.end(), sortByYAxis());
					
					// Left or Right Leg
					std::sort(trackers.begin(), trackers.begin()+2, sortByZAxis());
					initEndEffector(leftFoot, trackers[0]->getDeviceIndex(), trackers[0]->getDesPosition(), trackers[0]->getDesRotation());
					initEndEffector(rightFoot, trackers[1]->getDeviceIndex(), trackers[1]->getDesPosition(), trackers[1]->getDesRotation());
					
					// Hip
					initEndEffector(hip, trackers[2]->getDeviceIndex(), trackers[2]->getDesPosition(), trackers[2]->getDesRotation());
					
					// Left or Right Forearm
					std::sort(trackers.begin()+3, trackers.begin()+5, sortByZAxis());
					initEndEffector(leftForeArm, trackers[3]->getDeviceIndex(), trackers[3]->getDesPosition(), trackers[3]->getDesRotation());
					initEndEffector(rightForeArm, trackers[4]->getDeviceIndex(), trackers[4]->getDesPosition(), trackers[4]->getDesRotation());
				}
				
				
			} else if (vrDevice.trackedDevice == TrackedDevice::Controller) {
				// Hand controller
				if (devicePos.z() > 0) {
					initEndEffector(rightHand, i, devicePos, deviceRot);
				} else {
					initEndEffector(leftHand, i, devicePos, deviceRot);
				}
			}
		}
		
		// HMD
		SensorState stateLeftEye = VrInterface::getSensorState(0);
		SensorState stateRightEye = VrInterface::getSensorState(1);
		vec3 leftEyePos = stateLeftEye.pose.vrPose.position;
		vec3 rightEyePos = stateRightEye.pose.vrPose.position;
		vec3 hmdPosCenter = (leftEyePos + rightEyePos) / 2;
		initEndEffector(head, 0, hmdPosCenter, stateLeftEye.pose.vrPose.orientation);
	}
	
	void gamepadButton(int buttonNr, float value) {
		//log(Info, "gamepadButton buttonNr = %i value = %f", buttonNr, value);

		// Grip button => set size and reset an avatar to a default T-Pose
		if (buttonNr == 2 && value == 1) {
			calibratedAvatar = false;
			initTransAndRot();
			avatar->resetPositionAndRotation();
			setSize();
		}
		
		// Menu button => calibrate
		if (buttonNr == 1 && value == 1) {
			assignControllerAndTracker();
			calibrate();
			calibratedAvatar = true;
			log(Info, "Calibrate avatar");
		}
		
		// Trigger button => record data
		if (buttonNr == 33 && value == 1) {
			record();
		}
	}
	
	void initButtons() {
		VrPoseState controller;

		int count = 0;
		
		for (int i = 0; i < 16; ++i) {
			controller = VrInterface::getController(i);
			
			if (controller.trackedDevice == TrackedDevice::Controller) {
				Gamepad::get(i)->Button = gamepadButton;
				++count;
				log(Info, "Add gamepad controller %i", count);
			}
		}

		assert(count == 2);
		controllerButtonsInitialized = true;
	}
#endif
	void update() {
		float t = (float)(System::time() - startTime);
		double deltaT = t - lastTime;
		lastTime = t;
		
		// Move position of camera based on WASD keys
		if (S) cameraPos -= camForward * (float)deltaT * cameraMoveSpeed;
		if (W) cameraPos += camForward * (float)deltaT * cameraMoveSpeed;
		if (A) cameraPos += camRight * (float)deltaT * cameraMoveSpeed;
		if (D) cameraPos -= camRight * (float)deltaT * cameraMoveSpeed;
		
		Graphics4::begin();
		Graphics4::clear(Graphics4::ClearColorFlag | Graphics4::ClearDepthFlag, Graphics1::Color::Black, 1.0f, 0);
		Graphics4::setPipeline(pipeline);
		
#ifdef KORE_STEAMVR
		VrInterface::begin();

		if (!controllerButtonsInitialized) initButtons();
		
		VrPoseState vrDevice;
		for (int i = 0; i < numOfEndEffectors; ++i) {
			if (endEffector[i]->getDeviceIndex() != -1) {

				if (i == head) {
					SensorState state = VrInterface::getSensorState(0);

					// Get HMD position and rotation
					endEffector[i]->setDesPosition(state.pose.vrPose.position);
					endEffector[i]->setDesRotation(state.pose.vrPose.orientation);
				} else {
					vrDevice = VrInterface::getController(endEffector[i]->getDeviceIndex());

					// Get VR device position and rotation
					endEffector[i]->setDesPosition(vrDevice.vrPose.position);
					endEffector[i]->setDesRotation(vrDevice.vrPose.orientation);
				}

				executeMovement(i);
			}
		}
		
		// Render for both eyes
		SensorState state;
		for (int j = 0; j < 2; ++j) {
			VrInterface::beginRender(j);
			
			Graphics4::clear(Graphics4::ClearColorFlag | Graphics4::ClearDepthFlag, Graphics1::Color::Black, 1.0f, 0);
			
			state = VrInterface::getSensorState(j);
			
			renderAvatar(state.pose.vrPose.eye, state.pose.vrPose.projection);
			
			if (renderTrackerAndController) renderAllVRDevices();
			
			if (renderAxisForEndEffector) renderCSForEndEffector();
			
			if (renderRoom) renderLivingRoom(state.pose.vrPose.eye, state.pose.vrPose.projection);
			
			VrInterface::endRender(j);
		}
		
		VrInterface::warpSwap();
		
		Graphics4::restoreRenderTarget();
		Graphics4::clear(Graphics4::ClearColorFlag | Graphics4::ClearDepthFlag, Graphics1::Color::Black, 1.0f, 0);
		
		// Render on monitor
		mat4 P = getProjectionMatrix();
		mat4 V = getViewMatrix();

		if (!firstPersonMonitor) renderAvatar(V, P);
		else renderAvatar(state.pose.vrPose.eye, state.pose.vrPose.projection);
		
		if (renderTrackerAndController) renderAllVRDevices();
		
		if (renderAxisForEndEffector) renderCSForEndEffector();
		
		if (renderRoom) {
			if (!firstPersonMonitor) renderLivingRoom(V, P);
			else renderLivingRoom(state.pose.vrPose.eye, state.pose.vrPose.projection);
		}
#else
		// Read line
		float scaleFactor;
		Kore::vec3 desPosition[numOfEndEffectors];
		Kore::Quaternion desRotation[numOfEndEffectors];
		if (currentFile < numFiles) {
			bool dataAvailable = logger->readData(numOfEndEffectors, files[currentFile], desPosition, desRotation, scaleFactor);
			
			for (int i = 0; i < numOfEndEffectors; ++i) {
				endEffector[i]->setDesPosition(desPosition[i]);
				endEffector[i]->setDesRotation(desRotation[i]);
			}
			
			if (!calibratedAvatar) {
				avatar->resetPositionAndRotation();
				avatar->setScale(scaleFactor);
				calibrate();
				calibratedAvatar = true;
			}
			
			for (int i = 0; i < numOfEndEffectors; ++i) {
				executeMovement(i);
			}
			
			if (!dataAvailable) {
				currentFile++;
				calibratedAvatar = false;
			}
		} else {
			if (eval) {
				if (loop >= 0) {
					logger->saveEvaluationData(avatar);
					// log(Kore::Info, "%i more iterations!", loop);
					log(Kore::Info, "%s\t%i\t%f", files[currentFile], ikMode, evalValue[ikMode]);
					loop--;
					
					if (loop < 0) {
						logger->endEvaluationLogger();
						
						if (currentFile >= evalFilesInGroup - 1 && ikMode >= evalMaxIk && evalSteps <= 1)
							exit(0);
						else {
							if (evalSteps <= 1) {
								evalValue[ikMode] = evalInitValue[ikMode];
								evalSteps = evalStepsInit;
								ikMode++;
								endEffector[head]->setIKMode((IKMode)ikMode);
								endEffector[leftHand]->setIKMode((IKMode)ikMode);
								endEffector[rightHand]->setIKMode((IKMode)ikMode);
								endEffector[leftFoot]->setIKMode((IKMode)ikMode);
								endEffector[rightFoot]->setIKMode((IKMode)ikMode);
								endEffector[hip]->setIKMode((IKMode)ikMode);
							} else {
								evalValue[ikMode] += evalStep;
								evalSteps--;
							}
							
							if (ikMode > evalMaxIk) {
								ikMode = evalMinIk;
								currentFile++;
							}
							
							loop = 0;
							logger->startEvaluationLogger(files[currentFile], ikMode, lambda[ikMode], errorMaxPos[ikMode], errorMaxRot[ikMode], maxSteps[ikMode]);
						}
					}
				}
			}
		}
		
		// Get projection and view matrix
		mat4 P = getProjectionMatrix();
		mat4 V = getViewMatrix();
		
		renderAvatar(V, P);
		
		if (renderTrackerAndController) renderAllVRDevices();
		
		if (renderAxisForEndEffector) renderCSForEndEffector();
		
		if (renderRoom) renderLivingRoom(V, P, (float)deltaT);
#endif

		Graphics4::end();
		Graphics4::swapBuffers();
	}
	
	void keyDown(KeyCode code) {
		switch (code) {
			case Kore::KeyW:
				W = true;
				break;
			case Kore::KeyA:
				A = true;
				break;
			case Kore::KeyS:
				S = true;
				break;
			case Kore::KeyD:
				D = true;
				break;
			case Kore::KeyR:
#ifdef KORE_STEAMVR
				VrInterface::resetHmdPose();
#endif
				break;
			case KeyL:
				//Kore::log(Kore::LogLevel::Info, "cameraPos: (%f, %f, %f)", cameraPos.x(), cameraPos.y(), cameraPos.z());
				//Kore::log(Kore::LogLevel::Info, "camUp: (%f, %f, %f, %f)", camUp.x(), camUp.y(), camUp.z(), camUp.w());
				//Kore::log(Kore::LogLevel::Info, "camRight: (%f, %f, %f, %f)", camRight.x(), camRight.y(), camRight.z(), camRight.w());
				//Kore::log(Kore::LogLevel::Info, "camForward: (%f, %f, %f, %f)", camForward.x(), camForward.y(), camForward.z(), camForward.w());
				
				record();
				break;
			case Kore::KeyEscape:
			case KeyQ:
				System::stop();
				break;
			default:
				break;
		}
	}
	
	void keyUp(KeyCode code) {
		switch (code) {
			case Kore::KeyW:
				W = false;
				break;
			case Kore::KeyA:
				A = false;
				break;
			case Kore::KeyS:
				S = false;
				break;
			case Kore::KeyD:
				D = false;
				break;
			default:
				break;
		}
	}
	
	void mouseMove(int windowId, int x, int y, int movementX, int movementY) {
		Kore::Quaternion q1(vec3(0.0f, 1.0f, 0.0f), 0.01f * movementX);
		Kore::Quaternion q2(camRight, 0.01f * -movementY);
		
		camUp = q2.matrix() * camUp;
		camRight = q1.matrix() * camRight;
		
		q2.rotate(q1);
		mat4 mat = q2.matrix();
		camForward = mat * camForward;
	}
	
	
	void mousePress(int windowId, int button, int x, int y) {
		rotate = true;
	}
	
	void mouseRelease(int windowId, int button, int x, int y) {
		rotate = false;
	}
	
	void loadAvatarShader() {
		FileReader vs("shader.vert");
		FileReader fs("shader.frag");
		vertexShader = new Shader(vs.readAll(), vs.size(), VertexShader);
		fragmentShader = new Shader(fs.readAll(), fs.size(), FragmentShader);
		
		// This defines the structure of your Vertex Buffer
		structure.add("pos", Float3VertexData);
		structure.add("tex", Float2VertexData);
		structure.add("nor", Float3VertexData);
		
		pipeline = new PipelineState();
		pipeline = new PipelineState();
		pipeline->inputLayout[0] = &structure;
		pipeline->inputLayout[1] = nullptr;
		pipeline->vertexShader = vertexShader;
		pipeline->fragmentShader = fragmentShader;
		pipeline->depthMode = ZCompareLess;
		pipeline->depthWrite = true;
		pipeline->blendSource = Graphics4::SourceAlpha;
		pipeline->blendDestination = Graphics4::InverseSourceAlpha;
		pipeline->alphaBlendSource = Graphics4::SourceAlpha;
		pipeline->alphaBlendDestination = Graphics4::InverseSourceAlpha;
		pipeline->compile();
		
		tex = pipeline->getTextureUnit("tex");
		Graphics4::setTextureAddressing(tex, Graphics4::U, Repeat);
		Graphics4::setTextureAddressing(tex, Graphics4::V, Repeat);
		
		pLocation = pipeline->getConstantLocation("P");
		vLocation = pipeline->getConstantLocation("V");
		mLocation = pipeline->getConstantLocation("M");
	}
	
	void loadLivingRoomShader() {
		FileReader vs("shader_basic_shading.vert");
		FileReader fs("shader_basic_shading.frag");
		vertexShader_living_room = new Shader(vs.readAll(), vs.size(), VertexShader);
		fragmentShader_living_room = new Shader(fs.readAll(), fs.size(), FragmentShader);
		
		structure_living_room.add("pos", Float3VertexData);
		structure_living_room.add("tex", Float2VertexData);
		structure_living_room.add("nor", Float3VertexData);
		
		pipeline_living_room = new PipelineState();
		pipeline_living_room->inputLayout[0] = &structure_living_room;
		pipeline_living_room->inputLayout[1] = nullptr;
		pipeline_living_room->vertexShader = vertexShader_living_room;
		pipeline_living_room->fragmentShader = fragmentShader_living_room;
		pipeline_living_room->depthMode = ZCompareLess;
		pipeline_living_room->depthWrite = true;
		pipeline_living_room->blendSource = Graphics4::SourceAlpha;
		pipeline_living_room->blendDestination = Graphics4::InverseSourceAlpha;
		pipeline_living_room->alphaBlendSource = Graphics4::SourceAlpha;
		pipeline_living_room->alphaBlendDestination = Graphics4::InverseSourceAlpha;
		pipeline_living_room->compile();
		
		tex_living_room = pipeline_living_room->getTextureUnit("tex");
		Graphics4::setTextureAddressing(tex_living_room, Graphics4::U, Repeat);
		Graphics4::setTextureAddressing(tex_living_room, Graphics4::V, Repeat);
		
		pLocation_living_room = pipeline_living_room->getConstantLocation("P");
		vLocation_living_room = pipeline_living_room->getConstantLocation("V");
		mLocation_living_room = pipeline_living_room->getConstantLocation("M");
		mLocation_living_room_inverse = pipeline_living_room->getConstantLocation("MInverse");
		diffuse_living_room = pipeline_living_room->getConstantLocation("diffuseCol");
		specular_living_room = pipeline_living_room->getConstantLocation("specularCol");
		specular_power_living_room = pipeline_living_room->getConstantLocation("specularPow");
		lightPosLocation_living_room = pipeline_living_room->getConstantLocation("lightPos");
		lightCount_living_room = pipeline_living_room->getConstantLocation("numLights");
	}
	
	void init() {
		loadAvatarShader();
        avatar = new Avatar("avatar/avatar_male.ogex", "avatar/", structure);
		//avatar = new Avatar("avatar/avatar_female.ogex", "avatar/", structure);
		
		initTransAndRot();
		
		// Set camera initial position and orientation
		cameraPos = vec3(2.6, 1.8, 0.0);
		Kore::Quaternion q1(vec3(0.0f, 1.0f, 0.0f), Kore::pi / 2.0f);
		Kore::Quaternion q2(vec3(1.0f, 0.0f, 0.0f), -Kore::pi / 8.0f);
		camUp = q2.matrix() * camUp;
		camRight = q1.matrix() * camRight;
		q2.rotate(q1);
		mat4 mat = q2.matrix();
		camForward = mat * camForward;
		
		if (renderTrackerAndController) {
			viveObjects[0] = new MeshObject("vivemodels/vivetracker.ogex", "vivemodels/", structure, 1);
			viveObjects[1] = new MeshObject("vivemodels/vivecontroller.ogex", "vivemodels/", structure, 1);
		}
		
		if (renderTrackerAndController || renderAxisForEndEffector) {
			viveObjects[2] = new MeshObject("vivemodels/axis.ogex", "vivemodels/", structure, 1);
		}
		
		loadLivingRoomShader();
		train = new LivingRoom("train/train9.ogex", "train/", structure_living_room, 1);
		livingRoomRot = Kore::Quaternion(0, 0, 0, 1);
		livingRoomRot.rotate(Kore::Quaternion(vec3(1, 0, 0), -Kore::pi / 2.0));
		livingRoomRot.rotate(Kore::Quaternion(vec3(0, 0, 1), Kore::pi / 2.0));	
		train->M = mat4::RotationY(-0.0075) * mat4::Translation(0, -3, 0) * livingRoomRot.matrix().Transpose();

		floor = new LivingRoom("floor/floor.ogex", "floor/", structure_living_room, 1);
		
		for (int x = 0; x < 80; x++) {
			floorCoords[x] = mat4::Translation(5.5 + offsetXfloor * x, -3.75, -819 + offsetZfloor * x) * livingRoomRot.matrix().Transpose();
		}

		houseSmall = new LivingRoom("houseS/haus.ogex", "houseS/", structure_living_room, 1);

		houseMiddle = new LivingRoom("houseM/hausM.ogex", "houseM/", structure_living_room, 1);

		houseML = new LivingRoom("houseML/hausML.ogex", "houseML/", structure_living_room, 1);

		houseLarge = new LivingRoom("houseL/hausL.ogex", "houseL/", structure_living_room, 1);
		
		skybox = new LivingRoom("skybox/skybox.ogex", "skybox/", structure_living_room, 1);

		skybox->M = mat4::Translation(0, 0, -3.9) * livingRoomRot.matrix().Transpose();
		//skybox->setLights(lightCount_living_room, lightPosLocation_living_room);

		for (int x = 0; x < 110; x++) {
			//Fill the list with the position coordinates for all houses
			houseCoordsL[x] = mat4::Translation(22 + offsetXhouse * x, -2.9, -819 + offsetZhouse * x) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
			houseCoordsL2[x] = mat4::Translation(34 + offsetXhouse * x, -2.9, -810 + offsetZhouse * x) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
			houseCoordsL3[x] = mat4::Translation(46 + offsetXhouse * x, -2.9, -823 + offsetZhouse * x) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
			houseCoordsR[x] = mat4::Translation(-9 + offsetXhouse * x, -2.9, -819 + offsetZhouse * x) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
			houseCoordsR2[x] = mat4::Translation(-21 + offsetXhouse * x, -2.9, -813 + offsetZhouse * x) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
			houseCoordsR3[x] = mat4::Translation(-33 + offsetXhouse * x, -2.9, -825 + offsetZhouse * x) * livingRoomRot.matrix().Transpose() * mat4::RotationZ(1.585);
			//Fill list with numbers between 0-3 for the 4 different kinds of houses
			houseElementListL[x] = rand() % 4;
			houseElementListL2[x] = rand() % 4;
			houseElementListL3[x] = rand() % 4;
			houseElementListR[x] = rand() % 4;
			houseElementListR2[x] = rand() % 4;
			houseElementListR3[x] = rand() % 4;
		}
		
		logger = new Logger();
		
		endEffector = new EndEffector*[numOfEndEffectors];
		endEffector[head] = new EndEffector(headBoneIndex);
		endEffector[hip] = new EndEffector(hipBoneIndex);
		endEffector[leftHand] = new EndEffector(leftHandBoneIndex);
		endEffector[leftForeArm] = new EndEffector(leftForeArmBoneIndex);
		endEffector[rightHand] = new EndEffector(rightHandBoneIndex);
		endEffector[rightForeArm] = new EndEffector(rightForeArmBoneIndex);
		endEffector[leftFoot] = new EndEffector(leftFootBoneIndex);
		endEffector[rightFoot] = new EndEffector(rightFootBoneIndex);
		
#ifdef KORE_STEAMVR
		VrInterface::init(nullptr, nullptr, nullptr); // TODO: Remove
#endif
	}
}

int kore(int argc, char** argv) {
	System::init("BodyTracking", width, height);
	
	init();
	
	System::setCallback(update);
	
	startTime = System::time();
	
	Keyboard::the()->KeyDown = keyDown;
	Keyboard::the()->KeyUp = keyUp;
	Mouse::the()->Move = mouseMove;
	Mouse::the()->Press = mousePress;
	Mouse::the()->Release = mouseRelease;
	
	// Sound initiation
	Audio1::init();
	Audio2::init();
	startRecordingSound = new Sound("sound/start.wav");
	stopRecordingSound = new Sound("sound/stop.wav");
	
	System::start();
	
	return 0;
}
