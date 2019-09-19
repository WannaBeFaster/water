#include "../../headers.h"

#include "Patch.h"

//-------------------------------------------------------------------------------------------------
Patch::Patch()
{
    N_ = 0;
    step_ = 1.0f;
}

//-------------------------------------------------------------------------------------------------
Patch::~Patch()
{
}

//-------------------------------------------------------------------------------------------------
void Patch::init(int N, const glm::vec3& origin, GLfloat step)
{
    N_ = N;
    origin_ = origin;
    step_ = step;

    createVertices();
    createIndices();
}

//-------------------------------------------------------------------------------------------------
void Patch::createVertices()
{
    vertices_.clear();
    int count = N_*N_;
    vertices_.reserve(count);

    float x = origin_.x;
    float z = origin_.z;

    for (int j=0; j<N_; ++j, z+=step_)
    {
        x = origin_.x;

        for (int k=0; k<N_; ++k, x+=step_)
        {
            Vertex v;
            v.pos = glm::vec3(x, 0.0f, z);
            v.normal = glm::vec3(0, 1, 0);
            v.uv = glm::vec2(((float)k)/N_, ((float)j)/N_);
            vertices_.push_back(v);
        }
    }
}

//-------------------------------------------------------------------------------------------------
int Patch::getTrianglesCount() const
{
    // (N-1)*2 tris for every row (N-1 rows) + 4 zero tris * rows count - 2
    return ((N_-1)*2)*(N_-1) + 4*(N_-2);
}

//-------------------------------------------------------------------------------------------------
Patch::Triangle Patch::getTriangle(int i) const
{
    Triangle tri;

    // to keep CCW order we have to swap 2 first vertices for odd tris
    if (i&1)
    {
        tri.v1 = indices_[i+1];
        tri.v2 = indices_[i];
        tri.v3 = indices_[i+2];
    }
    else
    {
        tri.v1 = indices_[i];
        tri.v2 = indices_[i+1];
        tri.v3 = indices_[i+2];
    }

    return tri;
}

//-------------------------------------------------------------------------------------------------
void Patch::createIndices()
{
    // N = 4
    /*
    0---1---2---3
    |   |   |   |
    4---5---6---7
    |   |   |   |
    8---9--10--11
    |   |   |   |
    12--13--14--15

    Between 1st and 2nd row -> insert N, N+1 - to make 4 degenerated triangles

    0, 4, 1, 5, 2, 6, 3, 7,       7, 4,       4, 8, 5, 9, 6, 10, 7, 11,     11, 8, ...
    */

    indices_.clear();
    int count = (3 + (N_-2)*2 + 1)*(N_-1) + 2*(N_-2);
    indices_.resize(count);

    int i=0;
    int index=0;

    for (int y=0; y<N_-1; ++y)
    {
        indices_[i++] = index;
        indices_[i++] = index+N_;
        indices_[i++] = index+1;

        for (int x=0; x<N_-2; ++x)
        {
            indices_[i++] = index + x + 1 + N_;
            indices_[i++] = index + x + 2;
        }

        indices_[i++] = index + 2*N_ - 1;

        // add degenerated tris to connect rows
        if (y != N_-2)
        {
            indices_[i++] = index + 2*N_ - 1;
            indices_[i++] = index + N_;
        }

        index += N_;
    }
}
