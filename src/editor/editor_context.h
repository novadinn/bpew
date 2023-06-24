#ifndef EDITOR_CONTEXT_H
#define EDITOR_CONTEXT_H

#include "../ecs/scene.h"
#include "../ecs/entity.h"
#include "../graphics/camera.h"
#include "../graphics/framebuffer.h"

#include "glm/glm.hpp"

struct SpaceLayoutData;
struct SpaceModelingData;
struct SpaceShadingData;

struct EditorContext {    
    Scene *scene;
    Camera *main_camera;
    Entity selected_entity;

    SpaceLayoutData *space_layout_data;
    SpaceModelingData *space_modeling_data;
    SpaceShadingData *space_shading_data;
};

#endif // EDITOR_CONTEXT_H
