#pragma once


#include "pch.h"
//#include "Animator.h"

#include <Kore/Math/Quaternion.h>
#include <Kore/Log.h>

#include <iostream>
#include <string>
#include <ctime>
#include <fstream>

class Logger {
	
private:
	// Input and output file to for raw data
	std::fstream logDataReader;
	std::ofstream logdataWriter;

	// Output file to save data for MotionRecognition
	std::ofstream motionRecognitionWriter;

	// Output file to save data for hmm
	std::ofstream hmmWriter;
	std::ofstream hmmAnalysisWriter;
	
	// Output file to save data for evaluation
	std::fstream evaluationDataOutputFile;
	std::fstream evaluationConfigOutputFile;
	
public:
	Logger();
	~Logger();
	
	void startLogger(const char* filename);
	void endLogger();
	void fileReadClose();
	void saveData(const char* tag, Kore::vec3 rawPos, Kore::Quaternion rawRot, float scale);
	
	void startEvaluationLogger(const char* filename, int ikMode, float lambda, float errorMaxPos, float errorMaxRot, int maxSteps);
	//void saveEvaluationData(AnimatedEntity* entity, Animator* animator);
	void endEvaluationLogger();

	// Machine Learning Motion Recognition:
	// Create a new sensor reading table
	void startMotionRecognitionLogger(const char* filename);
	// Stop writing to the previously created sensor reading table
	void endMotionRecognitionLogger();
	// Write sensor data to the previously created sensor reading table
	void saveMotionRecognitionData(
		const char* tag, const char* subject, const char* activity,
		Kore::vec3 rawPos, Kore::vec3 desPos, Kore::vec3 finalPos,
		Kore::Quaternion rawRot, Kore::Quaternion desRot, Kore::Quaternion finalRot,
		Kore::vec3 rawAngVel, Kore::Quaternion desAngVel,
		Kore::vec3 rawLinVel, Kore::vec3 desLinVel,
		float scale, double time);

	// HMM
	void startHMMLogger(const char* filename, int num);
	void endHMMLogger();
	void saveHMMData(const char* tag, float lastTime, Kore::vec3 pos, Kore::Quaternion rot);
	void analyseHMM(const char* filename, double probability, bool newLine);
	
	//Load animation
	bool readData(const int numOfEndEffectors, const char* filename, Kore::vec3* rawPos, Kore::Quaternion* rawRot, float& scale);
};
