#pragma once

#include <glm/glm.hpp>

class Program
{
public:
    ~Program();

    GLuint getID() const {return id_;}

    void load(const std::string& vertexPath, const std::string& fragmentPath);

    void setUniform1i(const std::string& name, int val);
    void setUniform1f(const std::string& name, float val);
    void setUniformVec2(const std::string& name, const glm::vec2& val);
    void setUniformVec3(const std::string& name, const glm::vec3& val);
    void setUniformTexture(const std::string& name, GLuint tx, GLenum txLevel);
    void setUniformCubemapTexture(const std::string& name, GLuint tx, GLenum txLevel);

private:
    GLuint id_;
};
