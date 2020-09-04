#pragma once

#include "Level.h"

class TrainLevel :
    public Level
{
public:
    void update(double deltaT);
    void updateFPS(double deltaT);
    void updateBuilding(double deltaT, double speed);
    void l();
    void r();
    void v();
    void h();
    void x();
    void y();
    void z();
    void init();

    void controlsSetup();
    void audioSetup();
    void graphicsSetup();
    void checkHittingAvatar();
    void checkHittingEnemy();

    Level::ALevelObject* objects[1];
    bool levelStarted = true;
    double maxWaitintTime = 5.0;
    double countDown = 0.0;
    int poolSize = 3;
    float hittingHeight = 0.5;
    float hittingRadius = 0.3;
    Kore::mat4 locToGlob = Kore::mat4::RotationY(0.5 * Kore::pi) * Kore::mat4::RotationX(-0.5 * Kore::pi);

    float time;
    int fps = 0;;
    float offsets;

    void createEnemy(AnAnimatedEntity* reference, Kore::vec3 position, Kore::Quaternion rotation);
    void spawn(double deltaT);

    void checkEnemyCollision();

    Level::ALevelObject* createNewObject(String pfad, String pfad2, VertexStructure vstruct, float scale, Kore::vec3 pos, Kore::Quaternion rot);
    Level::ALevelObject* createObjectCopy(ALevelObject* object, Kore::vec3 pos, Kore::Quaternion rot);
    void skyInit(Kore::Graphics4::VertexStructure environmentSructure);
    void trainInit(Kore::Graphics4::VertexStructure environmentSructure);
    void groundInit(Kore::Graphics4::VertexStructure environmentSructure);
    void houseInit(Kore::Graphics4::VertexStructure environmentSructure);
    void airplaneInit(Kore::Graphics4::VertexStructure environmentSructure);
    void carInit(Kore::Graphics4::VertexStructure environmentSructure);
    void tunnelInit(Kore::Graphics4::VertexStructure environmentSructure);
};