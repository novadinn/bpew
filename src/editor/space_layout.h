#ifndef SPACE_LAYOUT_H
#define SPACE_LAYOUT_H

#include "../ecs/components/transform_component.h"
#include "../ecs/entity.h"
#include "draw_mode.h"
#include "event_receiver.h"

#include <glm/glm.hpp>

struct SpaceLayoutData {
  Framebuffer framebuffer;
  Framebuffer pp_framebuffer;
  glm::vec2 viewport_size{1, 1};
  glm::vec2 viewport_bounds[2];
  int gizmo_operation = -1;
  DrawMode draw_mode = DrawMode::SOLID;
  bool viewport_hovered = false;
  Entity hovered_entity;
};

EventReceiver *createSpaceLayoutReceiver();

void onCreateSpaceLayout(EditorContext *ctx);
void onDestroySpaceLayout(EditorContext *ctx);

void onUpdateSpaceLayout(EditorContext *ctx);
void onResizeSpaceLayout(EditorContext *ctx);

void onRenderBeginSpaceLayout(EditorContext *ctx);
void onRenderSpaceLayout(EditorContext *ctx);
void onRenderPostProcessingSpaceLayout(EditorContext *ctx);
void onRenderEndSpaceLayout(EditorContext *ctx);

void onDrawUIBeginSpaceLayout(EditorContext *ctx);
void onDrawUISpaceLayout(EditorContext *ctx);
void onDrawUIEndSpaceLayout(EditorContext *ctx);

#endif // SPACE_LAYOUT_H
