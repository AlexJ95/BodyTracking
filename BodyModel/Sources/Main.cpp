#pragma once

#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/System.h>
#include <Kore/Log.h>

#include "Settings.h"
#include "Level.h"
#include "Logger.h"
#include "InputController.h"
#include "AudioManager.h"

#include "TrainLevel.h"

#ifdef KORE_STEAMVR
#include <Kore/Vr/VrInterface.h>
#include <Kore/Vr/SensorState.h>
#include <Kore/Input/Gamepad.h>
#endif

using namespace Kore;

namespace {
	InputController* inputController;
	
	Logger* logger;
	
	double startTime;
	double lastTime;
	
	// Audio cues
	Sound* startRecordingSound;
	Sound* stopRecordingSound;

	AudioManager* audio;
	Level* currentLevel;
	
#ifdef KORE_STEAMVR
	bool controllerButtonsInitialized = false;
	float currentUserHeight;
	bool firstPersonMonitor = false;
#else
	int loop = 0;
#endif

	void record() {
		logRawData = !logRawData;

		if (logRawData) {
			audio->play("startRecordingSound");
			logger->startLogger("logData");
		} else {
			audio->play("stopRecordingSound");
			logger->endLogger();
		}
	}


	void update() {
		float t = (float)(System::time() - startTime);
		double deltaT = t - lastTime;
		lastTime = t;
		
		currentLevel->update(deltaT);
		inputController->update(deltaT);
	}
	
	void init() {
		logger = new Logger();
		
		currentLevel = new TrainLevel();
		currentLevel->init();

		/*
		inputController = new InputController({
		{KeyL, record},
		{KeyQ, System::stop}
			});
		*/

		// Sound initiation
		audio = new AudioManager({
				{"startRecordingSound", new Sound("sound/start.wav")},
				{"stopRecordingSound", new Sound("sound/stop.wav")}
			});

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

	System::start();
	
	return 0;
}
