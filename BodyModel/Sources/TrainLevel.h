#pragma once

#include "Level.h"

class TrainLevel :
    public Level
{
public:
    void update(double deltaT);
    void updatePoints();
    void updateAudio(double deltaT);
    void runCalibrationRoom();
    void deleteRoom();
    void loadTrainLevel();
    void gamePlay(double deltaT);
    void updateAnimatedEntity(double deltaT);
    void loadEnemies(double deltaT, int carriage);
    void loadEnemyRandom(double deltaT, int carriage);
    void loadTunnel(int range);
    void loadAirplane();
    void loadSign();
    void loadEnemy(int range, Kore::vec3 pos);
    bool enemySpawn(Kore::vec3 pos);
    void loadEnding();
    void updateFPS(double deltaT);
    void updateBuilding(double deltaT, double speed);
    void setPosition(ALevelObject* alo, float x, float y, float z);
	void x();
    void init();

    void controlsSetup();
    void audioSetup();
    void graphicsSetup();

    //void checkHittingAvatar();
    //void checkHittingEnemy();

    NonPlayerCharacter* currentEnemy;
    bool stationStarted = true;
    bool loaded = false;
    bool gameStart;
    int currentCarriage = 1;
    bool enemyExist = false;
    bool prepareforSpawn = false;
    int currentEnemyNumber = 0;

    //time between activating two enemies
    double maxWaitingTime = 5.0;
    double countDown = 0.0;

    //the maximal instances of enemies in the pool-list
    int poolSize = 10;
    //threshholds for the collision detection
    float hittingHeightFoot = 0.5;
    float hittingHeightHand = 1.0;
    float hittingRadius = 0.3;

    //used for the transformation of the position of the enemies to the world coordinate system
    Kore::mat4 locToGlob = Kore::mat4::RotationY(0.5 * Kore::pi) * Kore::mat4::RotationX(-0.5 * Kore::pi);

    double musicCountdown = 6;
    float starttime;
    float time;
    int fps = 0;;
    float offsets;
    
	bool tunnelActivated;
	int tunnelCounter;

   // void spawn(double deltaT);// , Kore::vec3 AirPlanePos);

//  Stashed changes
    int stationNr = 1;
    float stationLength = 15.0;
    bool stationComplete = false; 
    int maxEnemyCount = 1;  //sollte 1 sein 2 nur zum testen
    int currentEnemyCount = 0;
   // void checkStation(double deltaT);// , Kore::vec3 AirPlanePos);
    
    //airplane stuff
    Kore::vec3 airPlanePos;
    bool airplaneAtTheHeight = false;
   // void checkEnemyCollision(); 
    void showAttackInUI(string colorTag);
    //void checkingMoving();
    float minAttackingDistance = 3.0;
    float camVelocity = 0.1;

    void createEnemy(Kore::Graphics4::VertexStructure entitySructure);

    void freeMemory();

    Level::ALevelObject* createNewObject(String pfad, String pfad2, VertexStructure vstruct, float scale, Kore::vec3 pos, Kore::Quaternion rot);
    Level::ALevelObject* createObjectCopy(ALevelObject* object, Kore::vec3 pos, Kore::Quaternion rot);
    void roomInit(Kore::Graphics4::VertexStructure environmentSructure);
    void skyInit(Kore::Graphics4::VertexStructure environmentSructure);
	void signInit(Kore::Graphics4::VertexStructure environmentSructure);
	void trainInit(Kore::Graphics4::VertexStructure environmentSructure, int trainLenght);
    void groundInit(Kore::Graphics4::VertexStructure environmentSructure);
    void houseInit(Kore::Graphics4::VertexStructure environmentSructure);
    void airplaneInit(Kore::Graphics4::VertexStructure environmentSructure);
    void carInit(Kore::Graphics4::VertexStructure environmentSructure);
    void tunnelInit(Kore::Graphics4::VertexStructure environmentSructure);
    void reIteratorVector();
};