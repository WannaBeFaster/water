#include <GL/glew.h>
#include <glm/glm.hpp>

#include "tst_full.h"
#include "TestPhillips.h"
#include "TestScreenToNDC.h"
#include "TestFFTGPU.h"

#include <gtest/gtest.h>

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
