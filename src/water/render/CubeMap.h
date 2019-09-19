#pragma once

#include "Texture.h"

class CubeMap : public Texture
{
public:
    CubeMap() {}
    virtual ~CubeMap() {}

    virtual void load(const std::string& path);

protected:
    void loadSide(const std::string& path, GLenum target);
};
