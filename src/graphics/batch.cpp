#include "batch.h"

#include <glad/glad.h>

void Batch::create(int capacity, const std::vector<VertexAttribute> &attribs) {

  va.create();
  va.bind();

  vb.create(nullptr, capacity, GL_DYNAMIC_DRAW);

  va.addVertexBuffer(vb, attribs);
}

void Batch::destroy() {
  va.destroy();
  vb.destroy();
}

void Batch::flush(const std::vector<float> &vertices) {
  vb.bind();
  vb.flushSubData(&vertices[0], vertices.size() * sizeof(float));
  vb.unbind();
}