#ifndef RENDERER_H
#define RENDERER_H

// clang-format off
#include "renderer_context.h"
#include "shaders/shader.h"
#include "vertex_array.h"

#include <glm/glm.hpp>
#include <vector>
// clang-format on

struct Renderer {
  static void init();
  static void destroy();

  static void drawMeshMaterial(RendererContext *context);
  static void drawMeshSolid(RendererContext *context);
  static void drawMeshRendered(RendererContext *context);
  static void drawMeshWireframe(RendererContext *context);

  static void drawMeshVertices(RendererContext *context);

  static void applyFXAA(RendererContext *context);
  static void applyMeshOutline(RendererContext *context);
  static void applyVertexOutline(RendererContext *context);

  static void clear();
  static void setClearColor(const glm::vec4 &color);

private:
  static void bindMaterialUniforms(Material &material);
  static void unbindMaterialUniforms(Material &material);
};

#endif // RENDERER_H
