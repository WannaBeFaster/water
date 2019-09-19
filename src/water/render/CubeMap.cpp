#include "../../headers.h"

#include "../utils/Log.h"

#include "CubeMap.h"

//-------------------------------------------------------------------------------------------------
void CubeMap::load(const std::string& path)
{
    PLOG_INFO << "Loading cubemap: " << StreamColors(termcolor::cyan) << path;

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id_);

    loadSide(path + "/xpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    loadSide(path + "/xneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    loadSide(path + "/yneg.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    loadSide(path + "/ypos.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    //loadSide(path + "/ypos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    //loadSide(path + "/yneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    loadSide(path + "/zpos.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    loadSide(path + "/zneg.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

//-------------------------------------------------------------------------------------------------
void CubeMap::loadSide(const std::string& path, GLenum target)
{
    FIBITMAP* data = FreeImage_Load(FIF_PNG, path.c_str());
    int w = FreeImage_GetWidth(data);
    int h = FreeImage_GetHeight(data);
    FIBITMAP* pixels = FreeImage_ConvertTo32Bits(data);

    glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
    glTexImage2D(target, 0, GL_RGBA8, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(pixels));

    // free memory
    FreeImage_Unload(pixels);
    FreeImage_Unload(data);
}
