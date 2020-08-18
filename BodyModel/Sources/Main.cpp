#pragma once

#include "pch.h"

#include <Kore/IO/FileReader.h>
#include <Kore/System.h>
#include <Kore/Log.h>

#include "Settings.h"
#include "Level.h" //implicitly imported with trainlevel, but still in here for redundancy in case someone wants to extend this project and for abstraction
#include "Logger.h"
#include "InputController.h"
#include "AudioManager.h"

#include "TrainLevel.h"

#ifdef KORE_STEAMVR
#include <Kore/Vr/VrInterface.h>
#include <Kore/Vr/SensorState.h>
#include <Kore/Input/Gamepad.h>
#endif

namespace {
	InputController* inputController;
	AudioManager* audio;
	Logger* logger;
	Level* currentLevel;
	
	double startTime;
	double lastTime;
	
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
		float t = (float)(Kore::System::time() - startTime);
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
				{"startRecordingSound", new Kore::Sound("sound/start.wav")},
				{"stopRecordingSound", new Kore::Sound("sound/stop.wav")}
			});

#ifdef KORE_STEAMVR
		VrInterface::init(nullptr, nullptr, nullptr); // TODO: Remove
#endif
	}
}

int kore(int argc, char** argv) {
	Kore::System::init("BodyTracking", width, height);

	init();

	Kore::System::setCallback(update);
	startTime = Kore::System::time();

	Kore::System::start();
	
	return 0;
}
