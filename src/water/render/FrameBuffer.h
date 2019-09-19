#pragma once

#include "GLWrappers.h"

class Program;

//-------------------------------------------------------------------------------------------------
// Helper for rendering to a texture

class FrameBuffer
{
public:
    FrameBuffer();
    ~FrameBuffer();

    void init(int width, int height);
    GLuint getTextureID() const {return tx_;}
    void setProgram(Program* program);
    GLuint getID() const {return idBuffer_;}

    void render(std::function<void (Program&)> setUniforms);

private:
    Program* program_;

    GLuint idBuffer_;
    GLuint tx_;
    VertexArray vao_;
    Buffer vbo_;

    int width_;
    int height_;
};
