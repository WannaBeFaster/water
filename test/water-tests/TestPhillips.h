#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "../../src/water/anim/PhillipsSpectrum.h"

using namespace testing;

TEST(SpectrumTests, TestPhillips)
{
    PhillipsSpectrum spectrum;
    PhillipsSpectrum::Settings ps;

    ps.A = 100.0;
    ps.WindSpeed = 10.0;
    ps.WindAlignment = 2.0;
    ps.MinWaveLength = 0.05;
    ps.WindDirection = glm::normalize(glm::vec2(0.0, 1.0));

    spectrum.init(ps);
}
