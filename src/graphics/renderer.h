#ifndef RENDERER_H
#define RENDERER_H

#include "shaders/shader.h"
#include "vertex_array.h"
#include "model.h"
#include "../ecs/entity.h"
#include "../ecs/components/mesh_component.h"
#include "../ecs/components/light_component.h"
#include "../ecs/components/transform_component.h"
#include "../ecs/components/camera_component.h"

#include <glm/glm.hpp>
#include <vector>

struct Renderer {
    static void init();
    static void destroy();

    // TODO: can we pass an entt::view in here?
    static void drawMeshMaterial(uint32 entity_id, MeshComponent& mesh, glm::mat4& view_mat,
				 glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction,
				 const glm::mat4& model);
    static void drawMeshSolid(uint32 entity_id, MeshComponent& mesh, glm::mat4& view_mat,
			      glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction,
			      const glm::mat4& model);
    static void drawMeshRendered(uint32 entity_id, MeshComponent& mesh, glm::mat4& view_mat, 
				 glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction,
				 std::vector<LightComponent>& lights,
				 std::vector<TransformComponent>& light_transforms,
				 const glm::mat4& model);
    static void drawMeshWireframe(uint32 entity_id, MeshComponent& mesh, glm::mat4& view_mat,
				  glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction,
				  const glm::mat4& model);

    static void outlineSelectedMesh(uint color_texture_id, uint entities_texture_id,
				    glm::vec2 viewport_size,
				    uint32 selected_entity_id, glm::vec3 outline_color,
				    float mix_factor);
    
    static void clear();

    static void setClearColor(const glm::vec4& color);

private:
    static void setMaterialNodeUniforms(Shader& shader, Node& node);
    static void setMaterialNodeUniform(Shader& shader, Node& node, NodeInput& prop);
};

#endif // RENDERER_H
