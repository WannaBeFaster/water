#pragma once

#include <glm/glm.hpp>

class ScreenToNDC
{
public:
    static glm::vec2 convertToNDC(int x, int y, int viewWidth, int viewHeight);
};
