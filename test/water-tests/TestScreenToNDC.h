#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "../../src/water/render/ScreenToNDC.h"

using namespace testing;

TEST(ScreenToNDCTests, TestConvertLeft)
{
    int viewWidth = 1024;
    int viewHeight = 768;

    // 0, 0 -> -1.0, 1.0
    glm::vec2 res = ScreenToNDC::convertToNDC(0, 0, viewWidth, viewHeight);
    ASSERT_NEAR(res.x, -1.0, 1e-6);
    ASSERT_NEAR(res.y,  1.0, 1e-6);

    // 0, 768 -> -1.0, -1.0
    res = ScreenToNDC::convertToNDC(0, 768, viewWidth, viewHeight);
    ASSERT_NEAR(res.x, -1.0, 1e-6);
    ASSERT_NEAR(res.y, -1.0, 1e-6);
}

TEST(ScreenToNDCTests, TestConvertRight)
{
    int viewWidth = 1024;
    int viewHeight = 768;

    // 1024, 0 -> 1.0, 1.0
    glm::vec2 res = ScreenToNDC::convertToNDC(1024, 0, viewWidth, viewHeight);
    ASSERT_NEAR(res.x, 1.0, 1e-6);
    ASSERT_NEAR(res.y, 1.0, 1e-6);

    // 1024, 768 -> 1.0, -1.0
    res = ScreenToNDC::convertToNDC(1024, 768, viewWidth, viewHeight);
    ASSERT_NEAR(res.x, 1.0, 1e-6);
    ASSERT_NEAR(res.y, -1.0, 1e-6);
}
