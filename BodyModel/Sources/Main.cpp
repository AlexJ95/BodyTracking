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
	MachineLearningMotionRecognition* motionRecognizer;

	void record() {
		logRawData = !logRawData;
		
		if (!logRawData /*&& !motionRecognizer->isActive()*/) {
			audio->play("startRecordingSound");
			//logger->startLogger("logData");
		}
		else if (logRawData /*&& !motionRecognizer->isActive()*/) {
			audio->play("stopRecordingSound");
			//logger->endLogger();
		}
	}

	void x()	{ currentLevel->x(); }

	void init() {
#ifdef KORE_STEAMVR
		Kore::VrInterface::init(nullptr, nullptr, nullptr); // TODO: Remove
#endif
		logger = new Logger;
		motionRecognizer = motionRecognizer->getInstance();
		math = math->getInstance();

		inputController = inputController->getInstanceAndAppend({
				{Kore::KeyCode::KeyL, record},
				{Kore::KeyCode::KeyQ, Kore::System::stop},
				{Kore::KeyCode::KeyX, x},
			});
		inputController->setUI(ui);

		// Sound initiation
		//Kore::Sound* wav = new Kore::Sound("sound/maintitle.wav");
		//wav->length = 183.0f;
		//wav->setVolume(0.005f);

		audio = audio->getInstanceAndAppend({
				{"startRecordingSound",	new Kore::Sound("sound/start.wav")},
				{"stopRecordingSound",	new Kore::Sound("sound/stop.wav")},
				//{"titleSong",wav}
			});

		currentLevel = new TrainLevel();
		currentLevel->init();
		currentLevel->setUI(ui);
	}

	void update() {
		double t = Kore::System::time() - startTime;
		double deltaT = t - lastTime;
		lastTime = t;
		countTime += deltaT;
		double diff = 1 / fps;

		inputController->update(deltaT);
		currentLevel->update(deltaT);
		currentLevel->renderer->update(deltaT);
		
		/*
		if (countTime > diff) {
			countTime = countTime - diff;
			inputController->update(diff);
			currentLevel->update(diff);
		}*/
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