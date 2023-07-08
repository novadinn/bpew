#ifndef EDITOR_CONTEXT_H
#define EDITOR_CONTEXT_H

#include "../ecs/scene.h"
#include "../ecs/entity.h"
#include "../graphics/camera.h"
#include "../graphics/framebuffer.h"
#include "../graphics/renderer_context.h"
#include "editor_camera.h"

#include "glm/glm.hpp"

struct SpaceLayoutData;
struct SpaceModelingData;
struct SpaceShadingData;

struct EditorContext {
    RendererContext *renderer_context;
    Scene *scene;
    EditorCamera* editor_camera;
    Entity active_camera;
    Entity selected_entity;
    int selected_vertex_id;
    
    SpaceLayoutData *space_layout_data;
    SpaceModelingData *space_modeling_data;
    SpaceShadingData *space_shading_data;
};

#endif // EDITOR_CONTEXT_H
