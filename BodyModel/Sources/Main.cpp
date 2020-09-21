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

	//void f()	{ currentLevel->f(); }
	//void h()	{ currentLevel->h(); }
	//void t()	{ currentLevel->t(); }
	//void g()	{ currentLevel->g(); }
	void x()	{ currentLevel->x(); }
	//void v()	{ currentLevel->v(); }
	//void b()	{ currentLevel->b(); }
	//void n()	{ currentLevel->n(); }
	void p() { currentLevel->loadTunnel(0); }
	void signL() { currentLevel->triggerSignL(); }
	void signR() { currentLevel->triggerSignR(); }

	void init() {
		logger = new Logger;
		math = math->getInstance();

		inputController = inputController->getInstanceAndAppend({
				{Kore::KeyCode::KeyL, record},
				{Kore::KeyCode::KeyQ, Kore::System::stop},
				//{Kore::KeyCode::KeyF, f},
				//{Kore::KeyCode::KeyH, h},
				//{Kore::KeyCode::KeyV, v},
				//{Kore::KeyCode::KeyG, g},
				//{Kore::KeyCode::KeyT, t},
				{Kore::KeyCode::KeyX, x},
				//{Kore::KeyCode::KeyB, b},
				//{Kore::KeyCode::KeyN, n},
				{Kore::KeyCode::KeyP, p},
				{Kore::KeyCode::KeyI, signR},
				{Kore::KeyCode::KeyU, signL},
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
		countTime += deltaT;
		double diff = 1 / fps;

		if (countTime > diff) {
		countTime = countTime - diff;
		inputController->update(diff);
		Kore::vec3 currentPos = cameraPos;
		Kore::vec4 currentForward = camForward;
		double anyT = lastTime;
		currentLevel->update(diff);
		}
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