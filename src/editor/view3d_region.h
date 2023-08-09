#ifndef VIEW3D_REGION_H
#define VIEW3D_REGION_H

#include "../graphics/ping_pong_buffer.h"
#include "editor_context.h"

#include <glm/glm.hpp>

typedef enum {
  DM_WIREFRAME,
  DM_RENDERED,
  DM_SOLID,
  DM_MATERIAL_PREVIEW
} DrawMode;
typedef enum { V3D_OBJECT, V3D_EDIT } View3dMode;

struct View3dRegion {
  void createRenderTargets();
  void destroyRenderTargets();

  void onUpdate(EditorContext *ctx, void *space);
  void onResize(EditorContext *ctx);
  void onRender(EditorContext *ctx, void *space);
  void onRenderPostProcessing(EditorContext *ctx);
  void onDrawUI(EditorContext *ctx);

  Framebuffer framebuffer;
  PingPongBuffer ping_pong_buffer;

  glm::vec2 viewport_size{1, 1};
  glm::vec2 viewport_bounds[2];
  glm::vec2 mouse_position;
  bool viewport_hovered = false;

  int gizmo_operation = -1;
  int draw_mode = DM_SOLID;
  int v3d_mode = V3D_OBJECT;
};

#endif // VIEW3D_REGION_H