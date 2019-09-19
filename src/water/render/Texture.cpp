#include "../../headers.h"

#include "../utils/Log.h"

#include "Texture.h"

//-------------------------------------------------------------------------------------------------
Texture::Texture()
{
    id_ = GL_INVALID_VALUE;
}

//-------------------------------------------------------------------------------------------------
Texture::~Texture()
{
    if (id_ != GL_INVALID_VALUE)
        unload();
}

//-------------------------------------------------------------------------------------------------
void Texture::create()
{
    glGenTextures(1, &id_);
}

//-------------------------------------------------------------------------------------------------
void Texture::load(const std::string& path)
{     
    PLOG_INFO << "Loading texture: " << StreamColors(termcolor::cyan) << path;

    // create the texture
    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    FIBITMAP* data = FreeImage_Load(FIF_PNG, path.c_str());
    int width = FreeImage_GetWidth(data);
    int height = FreeImage_GetHeight(data);
    FIBITMAP* pixels = FreeImage_ConvertTo32Bits(data);

    // copy loaded data into the texture
    glTexImage2D(GL_TEXTURE_2D,
                 0,  // no mips, level 0
                 GL_RGBA8,
                 width,
                 height,
                 0,
                 GL_BGRA,
                 GL_UNSIGNED_BYTE,
                 FreeImage_GetBits(pixels));

    //
    FreeImage_Unload(pixels);
    FreeImage_Unload(data);

    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

//-------------------------------------------------------------------------------------------------
void Texture::unload()
{
    glDeleteTextures(1, &id_);
}

//-------------------------------------------------------------------------------------------------
void* Texture::getPixels32(const std::string& path)
{
    FIBITMAP* data = FreeImage_Load(FIF_PNG, path.c_str());
    int width = FreeImage_GetWidth(data);
    int height = FreeImage_GetHeight(data);
    FIBITMAP* pixels = FreeImage_ConvertTo32Bits(data);

    return pixels;
}
