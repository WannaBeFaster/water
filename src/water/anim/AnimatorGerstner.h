#pragma once

#include "Animator.h"

//-------------------------------------------------------------------------------------------------
// Gerstner waves

class AnimatorGerstner : public Animator
{
public:
    struct WaveSettings
    {
        // waves movement direction (vector length is the wind's velocity, meters/sec)
        glm::vec2 k;

        // unit wavevector
        glm::vec2 k1;

        // wave size, meters
        float L;

        // waves amplitude, meters
        float A;
    };

    AnimatorGerstner() {}

    void init();
    void addWave(const WaveSettings& s);

    virtual void update(double time);

protected:
    // initial (unanimated) mesh points positions (x0, y0 array)
    std::vector<glm::vec3> x0_;
    std::vector<WaveSettings> waves_;
};
