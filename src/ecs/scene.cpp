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
    CameraComponent* editor_camera = nullptr;
    TransformComponent* camera_tr = nullptr;
	
    auto group = registry.group<CameraComponent>(entt::get<TransformComponent>);
    for(auto entity : group) {
	auto [camera, transform] = group.get<CameraComponent, TransformComponent>(entity);

	if(camera.editor) {
	    editor_camera = &camera;
	    camera_tr = &transform;
	}
    }
	
    if(editor_camera) {
	auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
	for(auto entity : group) {
	    auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

	    Renderer::drawMeshWireframe((uint32)entity, mesh, (*editor_camera), (*camera_tr), transform.getModelMatrix());
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
    CameraComponent* editor_camera = nullptr;
    TransformComponent* camera_tr = nullptr;
	
    auto group = registry.group<CameraComponent>(entt::get<TransformComponent>);
    for(auto entity : group) {
	auto [camera, transform] = group.get<CameraComponent, TransformComponent>(entity);

	if(camera.editor) {
	    editor_camera = &camera;
	    camera_tr = &transform;
	}
    }

    if(editor_camera) {
	auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
	for(auto entity : group) {
	    auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			
	    Renderer::drawMeshSolid((uint32)entity, mesh, (*editor_camera), (*camera_tr), transform.getModelMatrix());
	}
    }
}

void Scene::onDrawMaterialPreview() {
    CameraComponent* editor_camera = nullptr;
    TransformComponent* camera_tr = nullptr;
	
    auto group = registry.group<CameraComponent>(entt::get<TransformComponent>);
    for(auto entity : group) {
	auto [camera, transform] = group.get<CameraComponent, TransformComponent>(entity);

	if(camera.editor) {
	    editor_camera = &camera;
	    camera_tr = &transform;
	}
    }

    if(editor_camera) {
	auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
	for(auto entity : group) {
	    auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			
	    Renderer::drawMeshMaterial((uint32)entity, mesh, (*editor_camera), (*camera_tr), transform.getModelMatrix());
	}
    }

}

void Scene::onUpdate() {
    auto view = registry.view<MeshComponent>();
    for(auto entity : view) {
	auto& mesh = view.get<MeshComponent>(entity);

	for(auto& material : mesh.materials) {
	    material.shader_builder.build_material_shader(material);
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
