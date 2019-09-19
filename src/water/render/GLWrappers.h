#pragma once

#include <functional>

//-------------------------------------------------------------------------------------------------
// OpenGL VAO

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    GLuint getID() const {return id_;}

    void bind();
    void unbind();

private:
    GLuint id_;
};

//-------------------------------------------------------------------------------------------------
class Buffer
{
public:
    Buffer();
    ~Buffer();

    GLuint getID() const {return id_;}

private:
    GLuint id_;
};
