#ifndef SPACE_LAYOUT_H
#define SPACE_LAYOUT_H

#include "../ecs/components/transform_component.h"
#include "../ecs/entity.h"
#include "../graphics/ping_pong_buffer.h"
#include "draw_mode.h"
#include "event_receiver.h"

#include <glm/glm.hpp>

struct SpaceLayoutData {
  Framebuffer framebuffer;
  PingPongBuffer ping_pong_buffer;
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

void onRenderSpaceLayout(EditorContext *ctx);
void onRenderPostProcessingSpaceLayout(EditorContext *ctx);

void onDrawUISpaceLayout(EditorContext *ctx);

#endif // SPACE_LAYOUT_H
