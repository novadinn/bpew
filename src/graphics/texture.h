#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>

struct Texture2D {
  void createFromFile(const char *p);
  void destroy();

  void bind();
  void unbind();

  inline uint getID() const { return id; }
  inline uint getWidth() const { return width; }
  inline uint getHeight() const { return height; }

private:
  uint id;
  uint width, height;
};

#endif
