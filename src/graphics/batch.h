#ifndef BATCH_H
#define BATCH_H

#include "index_buffer.h"
#include "vertex_array.h"
#include "vertex_attribute.h"
#include "vertex_buffer.h"

#include <vector>

struct Batch {
  void create(int capacity, const std::vector<VertexAttribute> &attribs);
  void destroy();

  void flush(const std::vector<float> &vertices);

  VertexArray va;
  VertexBuffer vb;
  /* TODO: IndexBuffer ib; */
  std::vector<VertexAttribute> attributes;
  int capacity;
};

#endif // BATCH_H