#include "../../headers.h"

#include "ScreenToNDC.h"
#include "../anim/Utils.h"

#include "ScreenRectangle.h"

//-------------------------------------------------------------------------------------------------
void ScreenRectangle::init(int rectLeftTopX, int rectLeftTopY, int rectWidth, int rectHeight, int viewWidth, int viewHeight)
{
    glm::vec2 lt = ScreenToNDC::convertToNDC(rectLeftTopX, rectLeftTopY, viewWidth, viewHeight);
    glm::vec2 rb = ScreenToNDC::convertToNDC(rectLeftTopX + rectWidth, rectLeftTopY + rectHeight, viewWidth, viewHeight);

    GLfloat g_vertex_buffer_data[] = {
            lt.x, lt.y, 0.0f,
            0.0f, 1.0f,
            lt.x, rb.y, 0.0f,
            0.0f, 0.0f,
            rb.x, rb.y, 0.0f,
            1.0f, 0.0f,
            lt.x, lt.y, 0.0f,
            0.0f, 1.0f,
            rb.x, rb.y, 0.0f,
            1.0f, 0.0f,
            rb.x, lt.y, 0.0f,
            1.0f, 1.0f
    };

    glBindVertexArray(vao_.getID());
    glBindBuffer(GL_ARRAY_BUFFER, vbo_.getID());
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------
void ScreenRectangle::setProgram(GLuint program)
{
    program_ = program;
    uTx_ = glGetUniformLocation(program_, "u_tx");
}

//-------------------------------------------------------------------------------------------------
void ScreenRectangle::draw()
{
    glBindVertexArray(vao_.getID());
    glUseProgram(program_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_.getID());

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tx_);
    glUniform1i(uTx_, 0);

    GLuint id = glGetUniformLocation(program_, "u_size");
    glUniform1i(id, txSize_);

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

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glBindVertexArray(0);
}
