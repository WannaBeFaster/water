#pragma once

class Patch
{
public:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    struct Triangle
    {
        GLuint v1, v2, v3;
    };

    Patch();
    ~Patch();

    // N - vertices count in grid side
    // step - cell length in world coords
    void init(int N, const glm::vec3& origin, GLfloat step);

    std::vector<Vertex>& getVertices() {return vertices_;}
    std::vector<GLuint>& getIndices() {return indices_;}

    int getTrianglesCount() const;
    Triangle getTriangle(int i) const;

    void animate(double time);

private:
    int N_;

    glm::vec3 origin_;
    GLfloat step_;

    std::vector<Vertex> vertices_;
    std::vector<GLuint> indices_;

    void createVertices();
    void createIndices();
};
