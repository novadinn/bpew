#ifndef SCENE_H
#define SCENE_H

#include "../core/platform.h"
#include "../core/uuid.h"
#include "entt/entt.hpp"

#include <glm/glm.hpp>
#include <string>

struct Entity;
struct RendererContext;

struct Scene {
  Entity createEntity(const std::string &name = std::string());
  Entity createEntityFromUUID(UUID uuid,
                              const std::string &name = std::string());
  void destroyEntity(Entity entity);

  void onDrawWireframe(RendererContext *context);
  void onDrawRendered(RendererContext *context);
  void onDrawSolid(RendererContext *context);
  void onDrawMaterialPreview(RendererContext *context);

  void onDrawMeshVertices(RendererContext *context);

  void onUpdateMaterialPreview();
  void onUpdateRendered();

  void searchIntersectedVertices(uint32 *entity_id, int *vertex_id,
                                 glm::vec3 ray_position,
                                 glm::vec3 ray_direction);

  void onResize(uint width, uint height);

  entt::registry &getEntityRegistry() { return registry; }

  void renameEntity(Entity entity, std::string name);

private:
  entt::registry registry;
};

#endif // SCENE_H
