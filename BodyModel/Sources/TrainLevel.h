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

    void createEnemy(AnAnimatedEntity* reference, Kore::vec3 position, Kore::Quaternion rotation);
};