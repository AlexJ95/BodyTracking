#pragma once

#include "Level.h"

class TrainLevel :
    public Level
{
public:
    void update(double deltaT);
    void runCalibrationRoom();
    void deleteRoom();
    void loadTrainLevel();
    void gamePlay(double deltaT);
    void loadEnemy(float deltaT, int carriage);
    void loadEnemyRandom(float deltaT, int carriage);
    void loadEnemy(float deltaT);
    void loadTunnel(int range);
    void loadEnding();
    void loadEnemy();
    void updateFPS(double deltaT);
    void updateBuilding(double deltaT, double speed);
    void setPosition(ALevelObject* alo, float x, float y, float z);
    void t();
    void f();
    void g();
    void h();
	void x();
    void v();
    void b();
    void n();
    void init();

    void controlsSetup();
    void audioSetup();
    void graphicsSetup();
    void checkHittingAvatar();
    void checkHittingEnemy();

    Level::ALevelObject* objects[1];
    bool stationStarted = true;

    bool gameStart;
    int currentCarriage = 1;
    bool enemyExist = false;

    double maxWaitintTime = 5.0;
    double countDown = 0.0;
    int poolSize = 10;
    float hittingHeightFoot = 0.5;
    float hittingHeightHand = 1.0;
    float hittingRadius = 0.3;
    Kore::mat4 locToGlob = Kore::mat4::RotationY(0.5 * Kore::pi) * Kore::mat4::RotationX(-0.5 * Kore::pi);

    float starttime;
    float time;
    int fps = 0;;
    float offsets;
// Updated upstream
    //void spawn(double deltaT);
	
	bool tunnelActivated;
	int tunnelCounter;

    void spawn(double deltaT);// , Kore::vec3 AirPlanePos);

//  Stashed changes
    int stationNr = 1;
    float stationLength = 30.0;
    bool stationComplete = false; 
    int maxEnemyCount = 1;  //sollte 1 sein 2 nur zum testen
    int currentEnemyCount = 0; 
    void checkStation(double deltaT);// , Kore::vec3 AirPlanePos);
    Kore::vec3 airPlanePos;
    bool airplaneAtTheHeight = false;
    void checkEnemyCollision(); 
    void showAttackInUI(string colorTag);
    void checkingMoving();
    float minAttackingDistance = 3.0;
    float camVelocity = 0.1;

    Level::ALevelObject* createNewObject(String pfad, String pfad2, VertexStructure vstruct, float scale, Kore::vec3 pos, Kore::Quaternion rot);
    Level::ALevelObject* createObjectCopy(ALevelObject* object, Kore::vec3 pos, Kore::Quaternion rot);
    void roomInit(Kore::Graphics4::VertexStructure environmentSructure);
    void skyInit(Kore::Graphics4::VertexStructure environmentSructure);
	void trainInit(Kore::Graphics4::VertexStructure environmentSructure, int trainLenght);
    void groundInit(Kore::Graphics4::VertexStructure environmentSructure);
    void houseInit(Kore::Graphics4::VertexStructure environmentSructure);
	void houseInit(Kore::Graphics4::VertexStructure environmentSructure, bool placeholder);
    void airplaneInit(Kore::Graphics4::VertexStructure environmentSructure);
    void carInit(Kore::Graphics4::VertexStructure environmentSructure);
    void tunnelInit(Kore::Graphics4::VertexStructure environmentSructure);
    void createEnemy(Kore::Graphics4::VertexStructure entitySructure);
    void freeMemory(ALevelObject* alo);
    void reIteratorVector();
};