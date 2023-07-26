#ifndef SPACE_MODELING_H
#define SPACE_MODELING_H

#include "../ecs/components/transform_component.h"
#include "../ecs/entity.h"
#include "../graphics/ping_pong_buffer.h"
#include "draw_mode.h"
#include "event_receiver.h"

#include <glm/glm.hpp>

struct SpaceModelingData {
  Framebuffer framebuffer;
  PingPongBuffer ping_pong_buffer;
  glm::vec2 viewport_size{1, 1};
  glm::vec2 viewport_bounds[2];
  int gizmo_operation = -1;
  DrawMode draw_mode = DrawMode::SOLID;
  bool viewport_hovered = false;
  glm::ivec2 mouse_position;
};

EventReceiver *createSpaceModelingReceiver();

void onCreateSpaceModeling(EditorContext *ctx);
void onDestroySpaceModeling(EditorContext *ctx);

void onUpdateSpaceModeling(EditorContext *ctx);
void onResizeSpaceModeling(EditorContext *ctx);

void onRenderSpaceModeling(EditorContext *ctx);
void onRenderPostProcessingSpaceModeling(EditorContext *ctx);

void onDrawUISpaceModeling(EditorContext *ctx);

#endif // SPACE_MODELING_H
