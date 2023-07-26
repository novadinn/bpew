#ifndef EDITOR_CONTEXT_H
#define EDITOR_CONTEXT_H

#include "../ecs/entity.h"
#include "../ecs/scene.h"
#include "../graphics/framebuffer.h"
#include "../graphics/renderer_context.h"
#include "editor_camera.h"

#include "glm/glm.hpp"

#include <map>

struct SpaceLayoutData;
struct SpaceModelingData;
struct SpaceShadingData;

struct EditorContext {
  RendererContext *renderer_context;
  Scene *scene;
  EditorCamera *editor_camera;
  Entity active_entity;
  std::vector<Entity> selected_entities;
  std::pair<Entity, int> active_vertex;
  std::vector<std::pair<Entity, int>> selected_vertices;

  SpaceLayoutData *space_layout_data;
  SpaceModelingData *space_modeling_data;
  SpaceShadingData *space_shading_data;

  bool entitySelected(Entity entity) {
    bool contains = false;
    for (int i = 0; i < selected_entities.size(); ++i) {
      if (selected_entities[i] == entity) {
        contains = true;
        break;
      }
    }

    return contains;
  }

  void destroyEntity() {
    scene->destroyEntity(active_entity);
    active_entity = {};

    for (int i = 0; i < selected_entities.size(); ++i) {
      if (selected_entities[i] == active_entity) {
        selected_entities.erase(selected_entities.begin() + i);
        break;
      }
    }
  }

  bool vertexSelected(std::pair<Entity, int> vertex) {
    bool contains = false;
    for (int i = 0; i < selected_vertices.size(); ++i) {
      if (selected_vertices[i] == vertex) {
        contains = true;
        break;
      }
    }

    return contains;
  }
};

#endif // EDITOR_CONTEXT_H
