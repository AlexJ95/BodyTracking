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

    void createEnemy(AnAnimatedEntity* reference, Kore::vec3 position, Kore::Quaternion rotation);
    Level::ALevelObject* createNewObject(String pfad, String pfad2, VertexStructure vstruct, float scale, Kore::vec3 pos, Kore::Quaternion rot);
};