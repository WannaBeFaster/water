#include "../../headers.h"

#include "../scene/Patch.h"

#include "../utils/Log.h"
#include "Utils.h"

using namespace std;

//-------------------------------------------------------------------------------------------------
void _check_gl_error(const char* file, int line)
{
        GLenum err = glGetError();

        if (err!=GL_NO_ERROR) {
                string error;

                switch(err) {
                        case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
                        case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
                        case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
                        case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
                        case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
                }

                PLOG_ERROR << "GL_" << error.c_str() << " - " << file << ":" << line;
                err = glGetError();
        }
        else
        {
            PLOG_INFO << "no GL error";
        }
}

//-------------------------------------------------------------------------------------------------
void Utils::saveHeightmap(const std::vector<Patch::Vertex>& vertices, int N, const std::string& path)
{
    FIBITMAP* bitmap = FreeImage_Allocate(N, N, 24);

    // find min and max
    double min = vertices[0].pos.y;
    double max = vertices[0].pos.y;

    int i=0;
    for(int z=0; z<N; z++)
    {
        for(int x=0; x<N; x++)
        {
            float y = vertices[i++].pos.y;
            if (y < min)
                min = y;
            if (y > max)
                max = y;
        }
    }

    double A = max - min;

    i=0;
    for(int z=0; z<N; z++)
    {
        for(int x=0; x<N; x++)
        {
            float y = vertices[i++].pos.y;
            int val = (int)255*(1.0f - (y - min)/A);

            RGBQUAD color;
            color.rgbBlue = color.rgbGreen = color.rgbRed = val;

            FreeImage_SetPixelColor(bitmap, x, z, &color);
        }
    }

    FreeImage_Save(FIF_BMP, bitmap, path.c_str(), 0);

    FreeImage_Unload(bitmap);
}

//-------------------------------------------------------------------------------------------------
void Utils::saveComplexMatrix(const std::vector<std::vector<Complex>>& data, const std::string& path, double Are, double Aim)
{
    FIBITMAP* bitmap = FreeImage_Allocate(data[0].size(), data.size(), 24);

    for(int z=0; z<data.size(); z++)
    {
        for(int x=0; x<data[z].size(); x++)
        {
            const Complex& n = data[z][x];

            RGBQUAD color;
            double r = 255*(1.0 - (n.Re + Are)/(2*Are));
            if (r < 0.0)
                r = 0.0;
            if (r > 255.0)
                r = 255.0;
            color.rgbRed = (uint)r;

            double g = 255*(1.0 - (n.Im + Aim)/(2*Aim));
            if (g < 0.0)
                g = 0.0;
            if (g > 255.0)
                g = 255.0;
            color.rgbGreen = (uint)g;

            color.rgbBlue = 0;

            FreeImage_SetPixelColor(bitmap, x, z, &color);
        }
    }

    FreeImage_Save(FIF_BMP, bitmap, path.c_str(), 0);

    FreeImage_Unload(bitmap);
}

//-------------------------------------------------------------------------------------------------
void Utils::saveDisplacementMap(const std::vector<std::vector<glm::vec2>>& data,
                                const std::string& pathRe,
                                const std::string& pathIm)
{
    int N = data.size();

    FIBITMAP* bitmapX = FreeImage_Allocate(N, N, 24);
    FIBITMAP* bitmapZ = FreeImage_Allocate(N, N, 24);

    // 1-й проход - найти минимум и максимум.
    double minX = data[0][0].x;
    double maxX = data[0][0].x;
    double minZ = data[0][0].y;
    double maxZ = data[0][0].y;

    for(int z=0; z<N; z++)
    {
        for(int x=0; x<N; x++)
        {
            const glm::vec2& y = data[z][x];
            if (y.x < minX)
                minX = y.x;
            if (y.x > maxX)
                maxX = y.x;
            if (y.y < minZ)
                minZ = y.y;
            if (y.y > maxZ)
                maxZ = y.y;
        }
    }

    double Ax = maxX - minX;
    double Ay = maxZ - minZ;

    for(int z=0; z<N; z++)
    {
        for(int x=0; x<N; x++)
        {
            const glm::vec2& n = data[z][x];

            int val = (int)255*(1.0f - (n.x - minX)/Ax);
            RGBQUAD color;
            color.rgbBlue = color.rgbGreen = color.rgbRed = val;
            FreeImage_SetPixelColor(bitmapX, x, z, &color);

            val = (int)255*(1.0f - (n.y - minZ)/Ay);
            color.rgbBlue = color.rgbGreen = color.rgbRed = val;
            FreeImage_SetPixelColor(bitmapZ, x, z, &color);
        }
    }

    FreeImage_Save(FIF_BMP, bitmapX, pathRe.c_str(), 0);
    FreeImage_Save(FIF_BMP, bitmapZ, pathIm.c_str(), 0);

    FreeImage_Unload(bitmapX);
    FreeImage_Unload(bitmapZ);
}
