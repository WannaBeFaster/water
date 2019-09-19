#pragma once

#include "../scene/Settings.h"
#include "../render/Program.h"
#include "../render/Transforms.h"

class CubeMap;

class Sky
{
public:
    Sky();

    void init(const Settings& settings, CubeMap* cubemap);
    void setTransforms(const Transforms& transforms);
    void draw();

private:
    CubeMap* cubemap_;
    Settings settings_;

    Program program_;
    VertexArray vao_;
    Buffer vbo_;

    glm::mat4 mvp_;
    glm::mat4 view_;

    void initGeo();
};
