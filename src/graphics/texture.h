#ifndef TEXTURE_H
#define TEXTURE_H

#include <map>
#include <string>
#include "../core/platform.h"

struct Texture2D {
    void createFromFile(const char* p);
    void destroy();

    void bind();
    void unbind();

    inline uint getID() const { return id; }
    inline uint getWidth() const { return width; }
    inline uint getHeight() const { return height; }

    static Texture2D* getTexture(std::string& path);    
    static bool haveTexture(std::string& path);
    static void addTexture(std::string path, Texture2D texture);	
private:
    uint id;
    uint width, height;
    
    static std::map<std::string, Texture2D> textures;
};

#endif
