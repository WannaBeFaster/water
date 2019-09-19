#pragma once

#include "Patch.h"
#include "../render/GLWrappers.h"
#include "../render/Program.h"
#include "../render/ScreenRectangle.h"
#include "../render/Transforms.h"
#include "../render/CubeMap.h"

#include "Settings.h"

class Animator;

class Ocean
{
public:
    void init(const Settings& settings);

    void update(double time);
    void setTransforms(const Transforms& transforms);
    void setCubemap(CubeMap* cubemap) {cubemap_ = cubemap;}

    void draw();

protected:
    void initAnimation();
    void initWater();

    void drawWater();

private:
    Settings settings_;

    float step_;
    Patch patch_;
    Animator* animator_;

    VertexArray vao_;
    Buffer      vbo_;
    Buffer      ebo_;

    Program program_;

    GLuint matrixID_;
    GLuint camPosID_;

    Transforms transforms_;
    CubeMap* cubemap_;
};
