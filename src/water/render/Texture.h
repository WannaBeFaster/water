#pragma once

#include <string>

class Texture
{
public:
    Texture();
    virtual ~Texture();

    virtual void create();

    virtual void load(const std::string& path);
    virtual void unload();

    GLuint getID() const {return id_;}

protected:
    void* getPixels32(const std::string& path);

    GLuint id_;
};
