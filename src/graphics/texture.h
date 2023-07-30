#ifndef TEXTURE_H
#define TEXTURE_H

#include "../core/platform.h"
#include <map>
#include <string>
#include <vector>

struct Texture2D {
  void createFromFile(const char *p);
  void createFromData(const unsigned char *data, uint w, uint h, uint chan_num);
  void destroy();

  void bind();
  void unbind();

  inline uint getID() const { return id; }
  inline uint getWidth() const { return width; }
  inline uint getHeight() const { return height; }
  inline uint getChanNumber() const { return chan_number; };

  static Texture2D *getTexture(uint index);
  static void addTexture(Texture2D texture);
  static bool validTextureIndex(uint index);

  // NOTE: All textures should be stored at here
  static std::vector<Texture2D> textures;

  std::string name;

private:
  uint id;
  uint width, height;
  uint chan_number;
};

#endif
