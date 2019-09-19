#include "../../headers.h"

#include "../anim/Utils.h"
#include "Program.h"

#include "FrameBuffer.h"

//-------------------------------------------------------------------------------------------------
// FrameBuffer
//-------------------------------------------------------------------------------------------------
FrameBuffer::FrameBuffer()
{
    program_ = nullptr;
    idBuffer_ = 0;
}

//-------------------------------------------------------------------------------------------------
FrameBuffer::~FrameBuffer()
{
    if (idBuffer_ != 0)
    {
        glDeleteFramebuffers(1, &idBuffer_);
        glDeleteTextures(1, &tx_);
    }
}

//-------------------------------------------------------------------------------------------------
void FrameBuffer::init(int width, int height)
{
    width_ = width;
    height_ = height;

    // create and bind FrameBuffer
    glGenFramebuffers(1, &idBuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, idBuffer_);

    // create render target
    glGenTextures(1, &tx_);
    glBindTexture(GL_TEXTURE_2D, tx_);

    glTexImage2D(GL_TEXTURE_2D,
                 0,  // no mips, level 0
                 GL_RGBA32F,
                 width,
                 height,
                 0,
                 GL_RGBA,
                 GL_FLOAT,
                 nullptr);

    // no need for filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tx_, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Failed to init frame buffer");

    GLfloat vertices[] = {
            -1.0f, 1.0f, 0.0f,
            0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,
            0.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f
    };

    glBindVertexArray(vao_.getID());
    glBindBuffer(GL_ARRAY_BUFFER, vbo_.getID());
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------
void FrameBuffer::setProgram(Program* program)
{
    program_ = program;
}

//-------------------------------------------------------------------------------------------------
void FrameBuffer::render(std::function<void(Program&)> setUniforms)
{
    // to store viewport size
    struct Viewport
    {
        GLint   x;
        GLint   y;
        GLsizei width;
        GLsizei height;
    } vp;
    glGetIntegerv(GL_VIEWPORT, (GLint*)&vp);

    glBindFramebuffer(GL_FRAMEBUFFER, idBuffer_);
    glViewport(0, 0, width_, height_);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Clear the screen buffers
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vao_.getID());
    glUseProgram(program_->getID());

    // callback
    setUniforms(*program_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_.getID());

    // positions
    glVertexAttribPointer(
            0,                  // attribute 0.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            5*sizeof(GLfloat),  // stride
            (void*)0            // array buffer offset
    );
    glEnableVertexAttribArray(0);

    // uv
    glVertexAttribPointer(
            1,                  // attribute 1.
            2,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            5*sizeof(GLfloat),  // stride
            (void*)(3*sizeof(GLfloat))            // array buffer offset
    );
    glEnableVertexAttribArray(1);

    // no need for filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindVertexArray(0);

    // restore render to the screen and viewport size
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(vp.x, vp.y, vp.width, vp.height);
}
