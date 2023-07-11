#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "index_buffer.h"
#include "vertex_attribute.h"
#include "vertex_buffer.h"

#include <vector>

struct VertexArray {
  void create();
  void destroy();

  void addVertexBuffer(VertexBuffer vb,
                       const std::vector<VertexAttribute> &attribs);
  void setIndexBuffer(IndexBuffer ib);

  void bind();
  void unbind();

  inline uint getID() const { return id; }

private:
  uint id;
};

#endif
