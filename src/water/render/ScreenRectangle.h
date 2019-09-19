#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "GLWrappers.h"

//-------------------------------------------------------------------------------------------------
// Helper to render textured rectangle in screen coordinates

class ScreenRectangle
{
public:
    void init(int rectLeftTopX, int rectLeftTopY, int rectWidth, int rectHeight, int viewWidth, int viewHeight);
    void setProgram(GLuint program);
    void setTexture(GLuint id, int txSize) {tx_ = id; txSize_ = txSize;}
    void draw();

private:
    GLuint program_;

    VertexArray vao_;
    Buffer vbo_;

    GLuint tx_;
    int txSize_;
    GLuint uTx_;
};
