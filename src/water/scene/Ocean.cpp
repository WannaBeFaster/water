#include "../../headers.h"

#include "../anim/AnimatorGerstner.h"
#include "../anim/AnimatorDFT.h"
#include "../anim/AnimatorDFT_GPU.h"
#include "../math/FourierTransform.h"
#include "../render/ScreenRectangle.h"

#include "Ocean.h"

//-------------------------------------------------------------------------------------------------
void Ocean::init(const Settings& settings)
{     
    settings_ = settings;

    initAnimation();
    initWater();
}

//-------------------------------------------------------------------------------------------------
void Ocean::initWater()
{
    // load shaders and prepare buffers
    std::string vertex, fragment;
    animator_->getProgramPaths(vertex, fragment);
    program_.load(vertex.c_str(), fragment.c_str());

    GLuint idVAO = vao_.getID();
    GLuint idVBO = vbo_.getID();
    GLuint idEBO = ebo_.getID();

    // fill VBO with initial vertices positions
    // Not using geometry shaders in this app

    glBindVertexArray(idVAO);
    glBindBuffer(GL_ARRAY_BUFFER, idVBO);
    auto& vertices = patch_.getVertices();
    int bytes = vertices.size()*sizeof(Patch::Vertex);
    glBufferData(GL_ARRAY_BUFFER, bytes, vertices.data(), GL_DYNAMIC_DRAW);

    // fill EBO with indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, patch_.getIndices().size()*sizeof(GLuint), patch_.getIndices().data(), GL_STATIC_DRAW);

    // pos
    glVertexAttribPointer(0,
                          3, GL_FLOAT,          // 3 floats
                          GL_FALSE,             // don't normalize
                          8*sizeof(GLfloat),    // stride
                          (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1,
                          3, GL_FLOAT,          // 3 floats
                          GL_FALSE,             // don't normalize
                          8*sizeof(GLfloat),    // stride
                          (GLvoid*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    // uv
    glVertexAttribPointer(2,
                          2, GL_FLOAT,          // 2 floats
                          GL_FALSE,
                          8*sizeof(GLfloat),
                          (GLvoid*)(2*sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // get uniforms handles
    matrixID_ = glGetUniformLocation(program_.getID(), "MVP");
    camPosID_ = glGetUniformLocation(program_.getID(), "u_camPos");
}

//-------------------------------------------------------------------------------------------------
void Ocean::update(double time)
{
    animator_->update(time);
}

//-------------------------------------------------------------------------------------------------
void Ocean::setTransforms(const Transforms& transforms)
{
    transforms_ = transforms;
}

//-------------------------------------------------------------------------------------------------
void Ocean::drawWater()
{
    glBindVertexArray(vao_.getID());
    glUseProgram(program_.getID());

    const glm::vec3& camPos = transforms_.CamPos;
    glm::mat4 mvp = transforms_.Proj*transforms_.View;

    glUniformMatrix4fv(matrixID_, 1, GL_FALSE, &mvp[0][0]);
    glUniform3f(camPosID_, camPos.x, camPos.y, camPos.z);

    animator_->setAdditionalUniforms(program_);

    program_.setUniformCubemapTexture("u_cubemap", cubemap_->getID(), GL_TEXTURE0);

    // fill VBO with new data
    glBindBuffer(GL_ARRAY_BUFFER, vbo_.getID());
    glBufferData(GL_ARRAY_BUFFER, patch_.getVertices().size()*sizeof(Patch::Vertex), patch_.getVertices().data(), GL_DYNAMIC_DRAW);

    glDrawElements(GL_TRIANGLE_STRIP, patch_.getIndices().size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------
void Ocean::draw()
{   
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    drawWater();

    // wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    animator_->draw();
}

//---------------------------------------------------------------------------------------------
void Ocean::initAnimation()
{   
    int N = settings_.GridResolution;
    float worldSize = settings_.GridWorldSize;
    auto& origin = settings_.GridOrigin;

    patch_.init(N, glm::vec3(origin.x, 0.0f, origin.y), worldSize/N);

    //---------------------------------------------------------------------------------------------
    // fully random heights
    //static AnimatorRandom anim;
    //anim.setPatch(&patch_);

    //---------------------------------------------------------------------------------------------
    // 1 sine wave
    /*
    static AnimatorSine anim;
    anim.setPatch(&patch_);
    */

    //---------------------------------------------------------------------------------------------
    // Gerstner waves animator
    /*
    static AnimatorGerstner anim;
    AnimatorGerstner::WaveSettings wave1;

    wave1.A = 20.0f;
    wave1.k = glm::vec2(1.0f, 0.0f);
    wave1.L = 20.0f;

    anim.setPatch(&patch_);
    anim.addWave(wave1);

    anim.init();
    */

    //---------------------------------------------------------------------------------------------
    // Inverse discrete Fourie transform based approach.
    //

    // CPU
    //static AnimatorRDFT anim;

    // GPU
    static AnimatorRDFT_GPU anim;

    anim.setPatch(&patch_);
    anim.init(settings_);

    //---------------------------------------------------------------------------------------------
    animator_ = &anim;
}
