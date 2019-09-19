#include "../../headers.h"

#include "../scene/Patch.h"
#include "AnimatorGerstner.h"

//-------------------------------------------------------------------------------------------------
void AnimatorGerstner::addWave(const WaveSettings& wave)
{
    waves_.push_back(wave);
}

//-------------------------------------------------------------------------------------------------
void AnimatorGerstner::init()
{
    // copy initial vertex positions
    auto& vertices = patch_->getVertices();
    x0_.reserve(vertices.size());
    x0_.clear();
    for(auto& v: vertices)
        x0_.push_back(v.pos);

    // prepare wave settings
    for (auto& wave: waves_)
    {
        // find k vector length from the wave length
        glm::vec2 k = glm::normalize(wave.k);
        k *= 2*glm::pi<float>()/wave.L;
        wave.k = k;

        // unit wavevector
        wave.k1 = glm::normalize(k);
    }
}

//-------------------------------------------------------------------------------------------------
void AnimatorGerstner::update(double t)
{
    // for all waves
    // X = X0 − (K/k)A*sin(K*X0 − wt)
    // y = A*cos(K*X0 − wt)

    // iterate both vertex arrays: inital and target
    auto it = patch_->getVertices().begin();
    for(auto& x0: x0_)
    {
        double dx = 0.0f;
        double dy = 0.0f;
        double dz = 0.0f;

        for(auto& wave: waves_)
        {
            const glm::vec2& k = wave.k;
            const glm::vec2& k1 = wave.k1;

            // w*w = gk (for deep ocean)
            double w = sqrt(9.81*glm::length(k));

            double A = wave.A;

            double arg = k.x*x0.x/2 + k.y*x0.z/2 - w*t;

            //
            double val = A*sin(arg);
            dx += k1.x*val;
            dz += k1.y*val;

            // wave height
            dy += A*cos(arg);
        }

        it->pos.x = x0.x - dx;
        it->pos.z = x0.z - dz;
        it->pos.y = x0.y + dy;

        it++;
    }

    generateNormals();
}
