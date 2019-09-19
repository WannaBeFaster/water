#pragma once

struct Transforms
{
    glm::mat4   Proj;
    glm::mat4   View;
    glm::vec3   CamPos;
    float       HorizAngle;
    float       VertAngle;
};
