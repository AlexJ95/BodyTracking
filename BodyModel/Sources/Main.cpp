#pragma once

#include "pch.h"

#include "Settings.h"
#include "Level.h" //implicitly imported with trainlevel, but still in here for redundancy in case someone wants to extend this project and for abstraction
#include "Logger.h"
#include "InputController.h"
#include "AudioManager.h"

#include "TrainLevel.h"

#include <Kore/IO/FileReader.h>
#include <Kore/System.h>
#include <Kore/Log.h>

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
	
	void init() {
		logger = new Logger;
		
		inputController = inputController->getInstanceAndAppend({
				{Kore::KeyCode::KeyL, record},
				{Kore::KeyCode::KeyQ, Kore::System::stop}
			});

		// Sound initiation
		audio = audio->getInstanceAndAppend({
				{"startRecordingSound", new Kore::Sound("sound/start.wav")},
				{"stopRecordingSound", new Kore::Sound("sound/stop.wav")}
			});

		currentLevel = new TrainLevel();
		currentLevel->init();

#ifdef KORE_STEAMVR
		VrInterface::init(nullptr, nullptr, nullptr); // TODO: Remove
#endif
	}

	void update() {
		float t = (float)(Kore::System::time() - startTime);
		double deltaT = t - lastTime;
		lastTime = t;

		inputController->update(deltaT);
		currentLevel->update(deltaT);
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