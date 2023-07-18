#include "index_buffer.h"

void IndexBuffer::create(const uint *inds, uint size) {
  count = size / sizeof(uint);

  glCreateBuffers(1, &id);

  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, size, inds, GL_STATIC_DRAW);
}

void IndexBuffer::destroy() { glDeleteBuffers(1, &id); }

void IndexBuffer::bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }

void IndexBuffer::unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
