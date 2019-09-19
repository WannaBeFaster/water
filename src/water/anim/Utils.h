#pragma once

#include "../scene/Patch.h"
#include "../math/Complex.h"

class Utils
{
public:
    static void saveHeightmap(const std::vector<Patch::Vertex>& vertices, int N, const std::string& path);
    static void saveComplexMatrix(const std::vector<std::vector<Complex>>& data, const std::string& path, double Are, double Aim);
    static void saveDisplacementMap(const std::vector<std::vector<glm::vec2>>& data,
                                    const std::string& pathDx,
                                    const std::string& pathDy);

    static GLuint loadCubemap();
};

void _check_gl_error(const char* file, int line);
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)
