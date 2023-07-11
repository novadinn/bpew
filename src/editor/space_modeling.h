#ifndef SPACE_MODELING_H
#define SPACE_MODELING_H

#include "../ecs/components/transform_component.h"
#include "../ecs/entity.h"
#include "event_receiver.h"
#include "draw_mode.h"

#include <glm/glm.hpp>

struct SpaceModelingData {
    Framebuffer framebuffer;
    Framebuffer pp_framebuffer;
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

void onRenderBeginSpaceModeling(EditorContext *ctx);
void onRenderSpaceModeling(EditorContext *ctx);
void onRenderPostProcessingSpaceModeling(EditorContext *ctx);
void onRenderEndSpaceModeling(EditorContext *ctx);

void onDrawUIBeginSpaceModeling(EditorContext *ctx);
void onDrawUISpaceModeling(EditorContext *ctx);
void onDrawUIEndSpaceModeling(EditorContext *ctx);

#endif // SPACE_MODELING_H
