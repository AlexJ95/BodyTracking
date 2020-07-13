#pragma once
#include "Level.h"
class TrainLevel :
    public Level
{
    virtual void Update(double deltaT);

    virtual void Init();
};