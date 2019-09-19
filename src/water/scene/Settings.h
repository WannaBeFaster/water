#pragma once

struct Settings
{
    Settings();

    //---------------------------------
    // basic
    // grid (and Fourie transform) resolution, must be power of 2.
    int GridResolution;

    // grid size in worlds units (they are not meters).
    float GridWorldSize;

    //
    glm::vec2 GridOrigin;

    // physical size of the grid, meters
    float Size;

    bool UsePhillips;

    //---------------------------------
    // Phillips spectrum settings
    // A coeff
    float A;

    // wind speed, meters/sec
    float WindSpeed;

    // must be odd
    float WindAlignment;

    // min wave size, meters. Lesser waves will be eliminated.
    float MinWaveLength;

    glm::vec2 WindDirection;

    double Choppiness;

    double DisplacementFactor;

    //---------------------------------
    // env & lighting

    // render sky if true
    bool RenderSky;

    // use [0.0, 1.0] for Phillips and 0.0 when using PM / Hasselmann spectrum
    float SmoothNormals;

    // path to the skybox cubemap textures, relative to data/tx/ ("sky1" for example).
    std::string SkyboxName;

    // no need to change it
    float SkyboxHalfSize;

    glm::vec3 OceanColor;
    glm::vec3 LightColor;
    glm::vec3 LightDir;
    glm::vec3 SkyColor;

    float Kr;
    float Kt;
    float Ks;

    // Phong factor
    float PhongFactor;
};
