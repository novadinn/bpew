#ifndef VERTEX_ATTRIBUTE_H
#define VERTEX_ATTRIBUTE_H

#include "../core/platform.h"

#include <cstdio>
#include <glad/glad.h>

struct VertexAttribute {

  VertexAttribute() {
    size = 0;
    count = 0;
    normalized = 0;
  }
  VertexAttribute(size_t size, uint count, GLenum normalized)
      : size(size), count(count), normalized(normalized) {}
  VertexAttribute(const VertexAttribute &attrib) {
    size = attrib.size;
    count = attrib.count;
    normalized = attrib.normalized;
  }

  inline size_t getSize() const { return size; }
  inline uint getCount() const { return count; }
  inline GLenum isNormalized() const { return normalized; }

  size_t size;
  uint count;
  GLenum normalized;
};

#endif // VERTEX_ATTRIBUTE_H
