#include "scene.h"

#include "entity.h"
#include "components.h"
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

void Scene::onDraw() {
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

			Renderer::drawModel(mesh.model, mesh.shader, main_camera->camera, transform.getModelMatrix());
		}
	}
}

void Scene::onResize(uint width, uint height) {
	auto view = registry.view<CameraComponent>();
	for(auto entity : view) {
		auto& camera = view.get<CameraComponent>(entity);

		camera.camera.setViewportSize(width, height);
	}
}
