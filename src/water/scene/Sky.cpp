#include "../../headers.h"

#include "../render/CubeMap.h"
#include "../render/GLWrappers.h"

#include "Sky.h"

//-------------------------------------------------------------------------------------------------
Sky::Sky()
{
}

//-------------------------------------------------------------------------------------------------
void Sky::init(const Settings& settings, CubeMap* cubemap)
{
    settings_ = settings;
    cubemap_ = cubemap;

    // init geo
    initGeo();

    // load shaders
    program_.load("data/shaders/Sky_vertex.glsl", "data/shaders/Sky_fragment.glsl");
}

//-------------------------------------------------------------------------------------------------
void Sky::setTransforms(const Transforms& transforms)
{
    //
    // Direction : Spherical coordinates to Cartesian coordinates conversion
    float h = transforms.HorizAngle;
    float v = transforms.VertAngle;

    glm::vec3 direction(
        cos(v) * sin(h),
        sin(v),
        cos(v) * cos(h)
    );

    // Right vector
    glm::vec3 right = glm::vec3(
        sin(h - 3.1415926f/2.0f),
        0.0f,
        cos(h - 3.1415926f/2.0f)
    );

    glm::vec3 up = glm::cross( right, direction );

    view_ = glm::lookAt(glm::vec3(0, 0, 0), direction, up);

    mvp_ = transforms.Proj*view_;
}

//-------------------------------------------------------------------------------------------------
void Sky::draw()
{
    glDisable(GL_DEPTH_TEST);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);

    glBindVertexArray(vao_.getID());
    glUseProgram(program_.getID());

    GLuint id = glGetUniformLocation(program_.getID(), "MVP");
    glUniformMatrix4fv(id, 1, GL_FALSE, &mvp_[0][0]);
    id = glGetUniformLocation(program_.getID(), "MV");
    glUniformMatrix4fv(id, 1, GL_FALSE, &view_[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_->getID());

    program_.setUniformVec3("u_skyColor", settings_.SkyColor);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_.getID());

    // positions
    glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            3*sizeof(float),    // stride
            (void*)0            // array buffer offset
    );
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------
void Sky::initGeo()
{
    float s = settings_.SkyboxHalfSize;

    float vertices[] = {
      -s,  s, -s,
      -s, -s, -s,
       s, -s, -s,
       s, -s, -s,
       s,  s, -s,
      -s,  s, -s,

      -s, -s,  s,
      -s, -s, -s,
      -s,  s, -s,
      -s,  s, -s,
      -s,  s,  s,
      -s, -s,  s,

       s, -s, -s,
       s, -s,  s,
       s,  s,  s,
       s,  s,  s,
       s,  s, -s,
       s, -s, -s,

      -s, -s,  s,
      -s,  s,  s,
       s,  s,  s,
       s,  s,  s,
       s, -s,  s,
      -s, -s,  s,

      -s,  s, -s,
       s,  s, -s,
       s,  s,  s,
       s,  s,  s,
      -s,  s,  s,
      -s,  s, -s,

      -s, -s, -s,
      -s, -s,  s,
       s, -s, -s,
       s, -s, -s,
      -s, -s,  s,
       s, -s,  s
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo_.getID());
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(vao_.getID());
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_.getID());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
}
