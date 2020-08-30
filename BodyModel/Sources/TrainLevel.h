#pragma once

#include "Level.h"

class TrainLevel :
    public Level
{
public:
    void update(double deltaT);

    void init();

    void controlsSetup();
    void audioSetup();
    void graphicsSetup();

    Level::ALevelObject* objects[6];
    int poolSize = 4;
    double countDown = 0.0;
    double maxWaitintTime = 5.0;
    bool levelStarted = true;

    void createEnemy(AnAnimatedEntity* reference, Kore::vec3 position, Kore::Quaternion rotation);
    void spawn(double deltaT);
    void checkEnemyCollision();
    Level::ALevelObject* createNewObject(String pfad, String pfad2, VertexStructure vstruct, float scale, Kore::vec3 pos, Kore::Quaternion rot);
};