#include "../../headers.h"

#include "ScreenToNDC.h"

//-------------------------------------------------------------------------------------------------
glm::vec2 ScreenToNDC::convertToNDC(int x, int y, int viewWidth, int viewHeight)
{
    float Xndc = 2.0f * x / viewWidth - 1.0f;
    float Yndc = 1.0f - 2.0f * y / viewHeight;

    return glm::vec2(Xndc, Yndc);
}
