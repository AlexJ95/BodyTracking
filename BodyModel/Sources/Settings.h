#pragma once

#include "pch.h"
#include <Kore/Math/Quaternion.h>

namespace {
	const int numFiles = 14;
    const char* files[numFiles] = {"walking.csv", "lunges.csv", "squats.csv", "kicks.csv", "yoga1.csv", "yoga2.csv", "yoga3.csv", "verticalChop.csv", "kick.csv", "horizontalSweep.csv", "falling.csv", "landing.csv", "dying1.csv", "dying2.csv" };
	int currentFile = 0;
	
	const float nearNull = 0.0001f;
	
	bool logRawData = false;
	
	// IK Parameter
	int ikMode = 0;
	float lambda[] =		{ 0.35f,	0.05f,		0.2f,		0.03f,		0.2f,		0.018f	};
	float errorMaxPos[] =	{ 0.01f,	0.1f,		0.001f,		0.01f,		0.001f,		0.01f	};
	float errorMaxRot[] =	{ 0.01f,	0.1f,		0.01f,		0.01f,		0.01f,		0.01f	};
	float maxSteps[] =		{ 10.0f,	100.0f,		20.0f,		10.0f,		20.0f,		60.0f	};
    
    // Evaluation values
    const bool eval = false;
    float* evalValue = lambda;
	float evalInitValue[] = { 0.35f,	0.05f,		0.2f,		0.03f,		0.2f,		0.018f };
	float evalStep = 0.1f;
	int evalSteps = 10;
    int evalFilesInGroup = numFiles;
	int evalMinIk = 0;
	int evalMaxIk = 5;
	int evalStepsInit = evalSteps;
	
	const int width = 1024;
	const int height = 768;

	const bool renderRoom = true;
	const int numOfEndEffectors = 8;

	bool firstPersonMonitor = false;

	bool hmdMode = false;

	double startTime;
	double lastTime;
	double countTime;
	float fps = 60;

	//Camera related stuff
	float cameraMoveSpeed = 6.0f;
	Kore::vec3 cameraPos = Kore::vec3(0, 0, 0);
	Kore::vec4 camUp = Kore::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	Kore::vec4 camForward = Kore::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	Kore::vec4 camRight = Kore::vec4(-1.0f, 0.0f, 0.0f, 0.0f);

	Kore::mat4 vrProjectionMatrix;
	Kore::mat4 vrViewMatrix;
}
