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

void Scene::onDrawWireframe() {
	CameraComponent* main_camera = nullptr;
	
	auto view = registry.view<CameraComponent>();
	for(auto entity : view) {
		auto& camera = view.get<CameraComponent>(entity);

		if(camera.main) {
			main_camera = &camera;
		}
	}
	
	if(main_camera) {
		auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
		for(auto entity : group) {
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

			Renderer::drawMeshWireframe((uint32)entity, mesh, (*main_camera), transform.getModelMatrix());
		}
	}
}

void Scene::onDrawRendered() {
	// CameraComponent* main_camera = nullptr;
	
	// auto view = registry.view<CameraComponent>();
	// for(auto entity : view) {
	// 	auto& camera = view.get<CameraComponent>(entity);

	// 	if(camera.main) {
	// 		main_camera = &camera;
	// 	}
	// }

	// if(main_camera) {

	// 	std::vector<LightComponent> lights;
	// 	std::vector<TransformComponent> light_transforms;

	// 	auto light_group = registry.group<LightComponent>(entt::get<TransformComponent>);
	// 	for(auto entity : light_group) {
	// 		auto [light, transform] = light_group.get<LightComponent, TransformComponent>(entity);

	// 		lights.push_back(light);
	// 		light_transforms.push_back(transform);
	// 	}
		
	// 	auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
	// 	for(auto entity : group) {
	// 		auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			
	// 		Renderer::drawMeshRendered((uint32)entity, mesh, (*main_camera), lights, light_transforms,
	// 						   transform.getModelMatrix());
	// 	}
	// }

  // TODO:
}

void Scene::onDrawSolid() {
	CameraComponent* main_camera = nullptr;
	
	auto view = registry.view<CameraComponent>();
	for(auto entity : view) {
		auto& camera = view.get<CameraComponent>(entity);

		if(camera.main) {
			main_camera = &camera;
		}
	}

	if(main_camera) {
		auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
		for(auto entity : group) {
			auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			
			Renderer::drawMeshSolid((uint32)entity, mesh, (*main_camera), transform.getModelMatrix());
		}
	}
}

void Scene::onDrawMaterialPreview() {
	// TODO:
}

void Scene::onResize(uint width, uint height) {
	auto view = registry.view<CameraComponent>();
	for(auto entity : view) {
		auto& camera = view.get<CameraComponent>(entity);

		camera.camera.setViewportSize(width, height);
	}
}
