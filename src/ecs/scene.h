#ifndef SCENE_H
#define SCENE_H

#include "../core/platform.h"
#include "entt/entt.hpp"

#include <glm/glm.hpp>
#include <string>

struct Entity;

struct Scene {
    Entity createEntity(const std::string& name = std::string());
    void destroyEntity(Entity entity);
	
    void onDrawWireframe(glm::mat4& view_mat, glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction);
    void onDrawRendered(glm::mat4& view_mat, glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction);
    void onDrawSolid(glm::mat4& view_mat, glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction);
    void onDrawMaterialPreview(glm::mat4& view_mat, glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction);

    void onUpdateMaterialPreview();
    void onUpdateRendered();
    
    void onResize(uint width, uint height);

    entt::registry& getEntityRegistry() { return registry; }

private:
    entt::registry registry;
};

#endif // SCENE_H
