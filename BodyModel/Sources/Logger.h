#pragma once


#include "pch.h"
//#include "Animator.h"
#include "InverseKinematics.h"

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
	void saveData(const char* tag, Kore::vec3 rawPos, Kore::Quaternion rawRot, float scale);
	
	void startEvaluationLogger(const char* filename, int ikMode, float lambda, float errorMaxPos, float errorMaxRot, int maxSteps);
	//void saveEvaluationData(AnimatedEntity* entity, Animator* animator);
	void endEvaluationLogger();
	
	// HMM
	void startHMMLogger(const char* filename, int num);
	void endHMMLogger();
	void saveHMMData(const char* tag, float lastTime, Kore::vec3 pos, Kore::Quaternion rot);
	void analyseHMM(const char* filename, double probability, bool newLine);
	
	//Load animation
	bool readData(const int numOfEndEffectors, const char* filename, Kore::vec3* rawPos, Kore::Quaternion* rawRot, float& scale);
};
