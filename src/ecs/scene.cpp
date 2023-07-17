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

void Scene::onDrawWireframe(RendererContext *context) {    	    
    auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto entity : group) {
	auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

	context->setCommonData((uint32)entity);
	context->setMeshData(&mesh, transform.getModelMatrix());

	Renderer::drawMeshWireframe(context);
    }    
}

void Scene::onDrawRendered(RendererContext *context) {		
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

	context->setCommonData((uint32)entity);
	context->setMeshData(&mesh, transform.getModelMatrix());
	context->setLightData(lights, light_transforms);
	
	Renderer::drawMeshRendered(context);
    }
	
}

void Scene::onDrawSolid(RendererContext *context) {       
    auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto entity : group) {
	auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

	context->setCommonData((uint32)entity);
	context->setMeshData(&mesh, transform.getModelMatrix());
	
	Renderer::drawMeshSolid(context);
    }    
}

void Scene::onDrawMaterialPreview(RendererContext *context) {        
    auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto entity : group) {
	auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

	context->setCommonData((uint32)entity);
	context->setMeshData(&mesh, transform.getModelMatrix());
	
	Renderer::drawMeshMaterial(context);
    }    
}

void Scene::onUpdateRendered() {
    std::vector<LightComponent> lights;
    uint num_point_lights = 0;
    uint num_spot_lights = 0;
    uint num_dir_lights = 0;
    
    auto light_view = registry.view<LightComponent>();
    for(auto entity : light_view) {
	auto& light = light_view.get<LightComponent>(entity);

	switch(light.type) {
	case LightComponent::LightType::SPOT:
	    num_spot_lights++;
	    break;
	case LightComponent::LightType::POINT:
	    num_point_lights++;
	    break;
	case LightComponent::LightType::DIRECTIONAL:
	    num_dir_lights++;
	    break;
	}	
    }

    ShaderBuilder::buildMaterialRenderedShader(MaterialManager::default_material, num_spot_lights,
						  num_point_lights, num_dir_lights);
    
    auto view = registry.view<MeshComponent>();
    for(auto entity : view) {
	auto& mesh = view.get<MeshComponent>(entity);	
	
	for(auto& material : mesh.materials) {
	    // TODO: update only when material is changed	    
	    Material* mat = MaterialManager::getMaterial(material);
	    if(mat) {
		ShaderBuilder::buildMaterialRenderedShader(*mat, num_spot_lights,
							   num_point_lights, num_dir_lights);	    
	    }	    	    
	}
    }    
}

void Scene::onUpdateMaterialPreview() {
    auto view = registry.view<MeshComponent>();

    ShaderBuilder::buildMaterialShader(MaterialManager::default_material);
    
    for(auto entity : view) {
	auto& mesh = view.get<MeshComponent>(entity);
       	
	for(auto& material : mesh.materials) {
	    Material* mat = MaterialManager::getMaterial(material);
	    if(mat) {
		ShaderBuilder::buildMaterialShader(*mat);	    
	    }
	}
    }    
}

void Scene::onDrawMeshVerticesOutlined(RendererContext *context) {
    auto group = registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto entity : group) {
	auto [transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);

	context->setCommonData((uint32)entity);
	context->setMeshData(&mesh, transform.getModelMatrix());
	
	Renderer::drawMeshVerticesOutlined(context);
    }
}

void Scene::onResize(uint width, uint height) {
    auto view = registry.view<CameraComponent>();
    for(auto entity : view) {
	auto& camera = view.get<CameraComponent>(entity);

	camera.setViewportSize(width, height);
    }
}

