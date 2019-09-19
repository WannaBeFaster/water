#pragma once

#include "../render/CubeMap.h"

#include "Ocean.h"
#include "Sky.h"

class Scene
{
public:

    void init(const Settings& settings);
    void update(double time);

    void setTransforms(const Transforms& transforms);
    void draw();

protected:
    Settings settings_;

    Ocean ocean_;
    Sky sky_;

    CubeMap cubemap_;
};
