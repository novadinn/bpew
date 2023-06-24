#include "scene.h"

#include "entity.h"
#include "components/tag_component.h"
#include "components/transform_component.h"
#include "components/mesh_component.h"
#include "components/camera_component.h"
#include "components/light_component.h"
#include "../graphics/renderer.h"

Entity Scene::createEntity(const std::string& name) {
	Entity entity;
	entity.create(registry.create(), this);
	
	entity.addComponent<TransformComponent>();
	TagComponent& tag = entity.addComponent<TagComponent>();
	// TODO: check if name doesnt already exists in the scene
	tag.tag = name.empty() ? "Entity" : name;

	return entity;
}

void Scene::destroyEntity(Entity entity) {
	registry.destroy(entity);
}

void Scene::onDrawWireframe(glm::mat4& view_mat, glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction) {    	    
    auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto entity : group) {
	auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

	Renderer::drawMeshWireframe((uint32)entity, mesh, view_mat, proj_mat, view_pos, direction, transform.getModelMatrix());
    }
    
}

void Scene::onDrawRendered(glm::mat4& view_mat, glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction) {		
    std::vector<LightComponent> lights;
    std::vector<TransformComponent> light_transforms;

    auto light_group = registry.group<LightComponent>(entt::get<TransformComponent>);
    for(auto entity : light_group) {
	auto [light, transform] = light_group.get<LightComponent, TransformComponent>(entity);

	lights.push_back(light);
	light_transforms.push_back(transform);
    }
		
    auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto entity : group) {
	auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			
	Renderer::drawMeshRendered((uint32)entity, mesh, view_mat, proj_mat, view_pos, direction,
				   lights, light_transforms,
				   transform.getModelMatrix());
    }
	
}

void Scene::onDrawSolid(glm::mat4& view_mat, glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction) {       
    auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto entity : group) {
	auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			
	Renderer::drawMeshSolid((uint32)entity, mesh, view_mat, proj_mat, view_pos, direction, transform.getModelMatrix());
    }    
}

void Scene::onDrawMaterialPreview(glm::mat4& view_mat, glm::mat4& proj_mat, glm::vec3& view_pos, glm::vec3& direction) {        
    auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto entity : group) {
	auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			
	Renderer::drawMeshMaterial((uint32)entity, mesh, view_mat, proj_mat, view_pos, direction, transform.getModelMatrix());
    }    
}

void Scene::onUpdate() {
    auto view = registry.view<MeshComponent>();
    for(auto entity : view) {
	auto& mesh = view.get<MeshComponent>(entity);

	for(auto& material : mesh.materials) {
	    ShaderBuilder::buildMaterialShader(material);
	}
    }
}

void Scene::onResize(uint width, uint height) {
    auto view = registry.view<CameraComponent>();
    for(auto entity : view) {
	auto& camera = view.get<CameraComponent>(entity);

	camera.setViewportSize(width, height);
    }
}

