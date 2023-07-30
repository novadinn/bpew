#include "texture.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <cstdio>
#include <glad/glad.h>

void Texture2D::createFromFile(const char *p) {
  glGenTextures(1, &id);

  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int w, h, chan_num;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(p, &w, &h, &chan_num, 0);

  if (data) {
    width = (uint)w;
    height = (uint)h;
    chan_number = chan_num;

    GLenum internal_fmt;
    GLenum data_fmt;
    if (chan_num == 4) {
      internal_fmt = GL_RGBA8;
      data_fmt = GL_RGBA;
    } else if (chan_num == 3) {
      internal_fmt = GL_RGB8;
      data_fmt = GL_RGB;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, width, height, 0, data_fmt,
                 GL_UNSIGNED_BYTE, data);
  } else {
    printf("Failed to load texture at path %s\n", p);
    return;
  }

  stbi_image_free(data);

  addTexture(*this);
}

void Texture2D::createFromData(const unsigned char *data, uint w, uint h,
                               uint chan_num) {
  glGenTextures(1, &id);

  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  width = w;
  height = h;
  chan_number = chan_num;

  GLenum internal_fmt;
  GLenum data_fmt;
  if (chan_num == 4) {
    internal_fmt = GL_RGBA;
    data_fmt = GL_RGBA;
  } else if (chan_num == 3) {
    internal_fmt = GL_RGB;
    data_fmt = GL_RGB;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, width, height, 0, data_fmt,
               GL_UNSIGNED_BYTE, data);

  addTexture(*this);
}

void Texture2D::destroy() { glDeleteTextures(1, &id); }

void Texture2D::bind() { glBindTexture(GL_TEXTURE_2D, id); }

void Texture2D::unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

std::vector<Texture2D> Texture2D::textures;

Texture2D *Texture2D::getTexture(uint index) {
  if (!validTextureIndex(index)) {
    return nullptr;
  }

  return &textures[index];
}

void Texture2D::addTexture(Texture2D texture) { textures.push_back(texture); }

bool Texture2D::validTextureIndex(uint index) {
  return index >= 0 && index < textures.size();
}
