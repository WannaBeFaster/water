#include "../../headers.h"
#include "../scene/Patch.h"
#include "Animator.h"

//-------------------------------------------------------------------------------------------------
// Animator
//-------------------------------------------------------------------------------------------------
void Animator::getProgramPaths(std::string& vertex, std::string& fragment)
{
    vertex = "data/shaders/WaterCPU_vertex.glsl";
    fragment = "data/shaders/WaterCPU_fragment.glsl";
}

//-------------------------------------------------------------------------------------------------
// AnimatorSine
//-------------------------------------------------------------------------------------------------
void AnimatorSine::update(double time)
{
    auto& vertices = patch_->getVertices();
    for (auto& vertex: vertices)
    {
        vertex.pos.y = 10*sin(time + 5*vertex.pos.x);
    }

    generateNormals();
}

//-------------------------------------------------------------------------------------------------
// AnimatorRandom
//-------------------------------------------------------------------------------------------------
void AnimatorRandom::update(double time)
{
    auto& vertices = patch_->getVertices();
    for (auto& vertex: vertices)
    {
        vertex.pos.y = rand() % 10;
    }

    generateNormals();
}

//-------------------------------------------------------------------------------------------------
// Animator
//-------------------------------------------------------------------------------------------------
void Animator::generateNormals()
{
    // key: vertex index
    // value: list of the normals to tris that have this vertex
    std::map<int, std::vector<glm::vec3>> nmap;

    // iterate all the tris
    // for each triangle (with nonzero area):
    // - find the normal
    int tris = patch_->getTrianglesCount();
    auto& vertices = patch_->getVertices();
    for (int i=0; i<tris; i++)
    {
        Patch::Triangle tri = patch_->getTriangle(i);

        // skip zero area
        if (tri.v1 == tri.v2 || tri.v2 == tri.v3 || tri.v3 == tri.v1)
            continue;

        glm::vec3 r1 = vertices[tri.v2].pos - vertices[tri.v1].pos;
        glm::vec3 r2 = vertices[tri.v3].pos - vertices[tri.v1].pos;

        glm::vec3 n = glm::normalize(glm::cross(r2, r1));

        nmap[tri.v1].push_back(n);
        nmap[tri.v2].push_back(n);
        nmap[tri.v3].push_back(n);
    }

    glm::vec3 res;
    for(int i=0; i<vertices.size(); i++)
    {
        auto& normals = nmap[i];

        res = glm::vec3(0.0);
        for(auto& ni: normals)
        {
            res = res + ni;
        }

        vertices[i].normal = glm::normalize(res);
    }
}
