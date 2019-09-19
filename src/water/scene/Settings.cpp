#include "../../headers.h"

#include "Settings.h"

//-------------------------------------------------------------------------------------------------
Settings::Settings()
{
    GridWorldSize = 1000.0f;
    GridOrigin = glm::vec2(-500.0f, -500.0f);

    GridResolution = 256;

    Size = 250.0f;

    UsePhillips = true;

    A = 0.000004;
    WindSpeed = 20.0f;
    WindAlignment = 2.0f;
    MinWaveLength = 1.0f;
    WindDirection = glm::normalize(glm::vec2(1.0f, 0.0f));

    Choppiness = 1.2f;
    DisplacementFactor = 0.5f;

    RenderSky = false;
    SkyboxName = "sky1";
    SkyboxHalfSize = 500.0f;
    SmoothNormals = 1.0f;

    OceanColor = glm::vec3(0x38/255.0, 0x6d/255.0, 0x87/255.0); // 0x081d37

    LightColor = glm::vec3(1.0, 1.0, 0.8);

    SkyColor = glm::vec3(0.0, 1.0, 1.0);

    // vector to the light
    LightDir = glm::normalize(glm::vec3(100, 100, -20));

    Kr = 1.0f;
    Kt = 1.0f;
    Ks = 0.0f;

    PhongFactor = 1200.0f;
}
