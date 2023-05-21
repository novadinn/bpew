#ifndef RENDERER_H
#define RENDERER_H

#include "shader.h"
#include "vertex_array.h"
#include "model.h"
#include "camera.h"
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

	// TODO: this shoud take in a material
	// TODO: should it lights?
	// TODO: can we pass a entt::view in here?
	static void drawMesh(MeshComponent& mesh, CameraComponent& camera,
						 std::vector<LightComponent>& lights,
						 std::vector<TransformComponent>& light_transforms,
						 const glm::mat4& model);
	static void drawMeshSolid(MeshComponent& mesh, CameraComponent& camera,
							  const glm::mat4& model);
	static void drawMeshRendered(MeshComponent& mesh, CameraComponent& camera,
								 std::vector<LightComponent>& lights,
								 std::vector<TransformComponent>& light_transforms,
								 const glm::mat4& model);
	static void drawMeshWireframe(MeshComponent& mesh, CameraComponent& camera,
								  const glm::mat4& model);
	
	static void clear();

	static void setClearColor(const glm::vec4& color);
};

#endif // RENDERER_H
