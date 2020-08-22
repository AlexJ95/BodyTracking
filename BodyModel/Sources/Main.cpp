#pragma once

#include "pch.h"

#include "Settings.h"
#include "Level.h" //implicitly imported with trainlevel, but still in here for redundancy in case someone wants to extend this project and for abstraction
#include "Logger.h"
#include "InputController.h"
#include "AudioManager.h"
#include "UI3D.h"

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
	UI3D* ui;
	Kore::Window* window;
	
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

	void keyDown(Kore::KeyCode code) {
		inputController->keyDown(code);
	}

	void keyUp(Kore::KeyCode code) {
		inputController->keyUp(code);
	}

	void mouseMove(int windowId, int x, int y, int movementX, int movementY) {
		inputController->mouseMove(windowId, x, y, movementX, movementY);
	}

	void mousePress(int windowId, int button, int x, int y) {
		inputController->mousePress(windowId, button, x, y);
	}

	void mouseRelease(int windowId, int button, int x, int y) {
		inputController->mouseRelease(windowId, button, x, y);
	}
	
	void init() {
		logger = new Logger;
		
		inputController = inputController->getInstanceAndAppend({
				{Kore::KeyCode::KeyL, record},
				{Kore::KeyCode::KeyQ, Kore::System::stop}
			},ui);
		

		// Sound initiation
		audio = audio->getInstanceAndAppend({
				{"startRecordingSound", new Kore::Sound("sound/start.wav")},
				{"stopRecordingSound", new Kore::Sound("sound/stop.wav")}
			});

		currentLevel = new TrainLevel();
		currentLevel->init();
		currentLevel->setUI(ui);

		Kore::Keyboard::the()->KeyDown = keyDown;
		Kore::Keyboard::the()->KeyUp = keyUp;
		Kore::Mouse::the()->Move = mouseMove;
		Kore::Mouse::the()->Press = mousePress;
		Kore::Mouse::the()->Release = mouseRelease;
		
#ifdef KORE_STEAMVR
		VrInterface::init(nullptr, nullptr, nullptr); // TODO: Remove
#endif
	}

	void update() {
		float t = (float)(Kore::System::time() - startTime);
		double deltaT = t - lastTime;
		lastTime = t;

		currentLevel->update(deltaT);
		inputController->update(deltaT);
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
