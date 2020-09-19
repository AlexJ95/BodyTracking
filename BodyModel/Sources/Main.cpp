#pragma once

#include "pch.h"
#include <iostream>

#include "Settings.h"
#include "Level.h" //implicitly imported with trainlevel, but still in here for redundancy in case someone wants to extend this project and for abstraction
#include "Logger.h"
#include "InputController.h"
#include "AudioManager.h"
#include "UI3D.h"

#include "TrainLevel.h"

#include <Kore/System.h>
#include <Kore/Log.h>

#ifdef KORE_STEAMVR
#include <Kore/Vr/VrInterface.h>
#endif

namespace {
	InputController* inputController;
	AudioManager* audio;
	CustomMath* math;
	Logger* logger;
	Level* currentLevel;
	UI3D* ui;
	Kore::Window* window;

	void record() {
		logRawData = !logRawData;
		
		if (!logRawData /*&& !motionRecognizer->isActive()*/) {
			audio->play("startRecordingSound");
			logger->startLogger("logData");
		}
		else if (logRawData /*&& !motionRecognizer->isActive()*/) {
			audio->play("stopRecordingSound");
			logger->endLogger();
		}
	}

	void links()	{ currentLevel->l(); }
	void rechts()	{ currentLevel->r(); }
	void vorne()	{ currentLevel->v(); }
	void hinten()	{ currentLevel->h(); }
	void tunnel()	{ currentLevel->t(); }
	void xachse()	{ currentLevel->x(); }
	void yachse()	{ currentLevel->y(); }
	void zachse()	{ currentLevel->z(); }

	void init() {
		logger = new Logger;
		math = math->getInstance();

		inputController = inputController->getInstanceAndAppend({
				{Kore::KeyCode::KeyL, record},
				{Kore::KeyCode::KeyQ, Kore::System::stop},
				{Kore::KeyCode::KeyF, links},
				{Kore::KeyCode::KeyH, rechts},
				{Kore::KeyCode::KeyV, vorne},
				{Kore::KeyCode::KeyG, hinten},
				{Kore::KeyCode::KeyT, tunnel},
				{Kore::KeyCode::KeyX, xachse},
				{Kore::KeyCode::KeyB, yachse},
				{Kore::KeyCode::KeyN, zachse},
			});
		inputController->setUI(ui);

		// Sound initiation
		audio = audio->getInstanceAndAppend({
				{"startRecordingSound",	new Kore::Sound("sound/start.wav")},
				{"stopRecordingSound",	new Kore::Sound("sound/stop.wav")}
			});

		currentLevel = new TrainLevel();
		currentLevel->init();
		currentLevel->setUI(ui);

#ifdef KORE_STEAMVR
		Kore::VrInterface::init(nullptr, nullptr, nullptr); // TODO: Remove
#endif
	}

	void update() {
		double t = Kore::System::time() - startTime;
		double deltaT = t - lastTime;
		lastTime = t;

		inputController->update(deltaT);
		Kore::vec3 currentPos = cameraPos;
		Kore::vec4 currentForward = camForward;
		double anyT = lastTime;
		currentLevel->update(deltaT);
	}
}

int kore(int argc, char** argv) {
	window = Kore::System::init("BodyTracking", width, height);
	ui = new UI3D(window);
	init();
	Kore::System::setShutdownCallback(UIshutDown);
	Kore::System::setCallback(update);
	startTime = Kore::System::time();

	Kore::System::start();
	
	return 0;
}