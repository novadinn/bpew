#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "../core/platform.h"

#include <glad/glad.h>

struct VertexBuffer {
  void create(const void *verts, uint size);
  void destroy();

  void bind();
  void unbind();

  inline uint getId() const { return id; }

private:
  uint id;
};

#endif // VERTEX_BUFFER_H
