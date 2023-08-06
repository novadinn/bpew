#include "vertex_buffer.h"

void VertexBuffer::create(const void *verts, uint size, GLenum usage) {
  glCreateBuffers(1, &id);
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, size, verts, usage);
}

void VertexBuffer::destroy() { glDeleteBuffers(1, &id); }

void VertexBuffer::bind() { glBindBuffer(GL_ARRAY_BUFFER, id); }

void VertexBuffer::unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
