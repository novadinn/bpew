#ifndef RENDERER_H
#define RENDERER_H

#include "../ecs/components/camera_component.h"
#include "../ecs/components/light_component.h"
#include "../ecs/components/mesh_component.h"
#include "../ecs/components/transform_component.h"
#include "../ecs/entity.h"
#include "renderer_context.h"
#include "shaders/shader.h"
#include "vertex_array.h"

#include <glm/glm.hpp>
#include <vector>

struct Renderer {
  static void init();
  static void destroy();

  static void drawMeshMaterial(RendererContext *context);
  static void drawMeshSolid(RendererContext *context);
  static void drawMeshRendered(RendererContext *context);
  static void drawMeshWireframe(RendererContext *context);

  /* NOTE: unlike meshes drawing code, outlining is done in a single pass here
   */
  static void drawMeshVerticesOutlined(RendererContext *context);

  static void applyFXAA(RendererContext *context);
  static void applyMeshOutline(RendererContext *context);

  static void clear();
  static void setClearColor(const glm::vec4 &color);

private:
  static void bindMaterialUniforms(Material &material);
  static void unbindMaterialUniforms(Material &material);
};

#endif // RENDERER_H
