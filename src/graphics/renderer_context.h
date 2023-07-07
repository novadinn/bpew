#ifndef RENDERER_CONTEXT_H
#define RENDERER_CONTEXT_H

#include "renderer_context.h"
#include "shaders/shader.h"
#include "vertex_array.h"
#include "../ecs/entity.h"
#include "../ecs/components/mesh_component.h"
#include "../ecs/components/light_component.h"
#include "../ecs/components/transform_component.h"
#include "../ecs/components/camera_component.h"

#include <glm/glm.hpp>
#include <vector>

struct RendererContext {
    uint32 current_entity_id;
    
    MeshComponent *mesh;
    glm::mat4 model;

    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 view_pos;
    glm::vec3 direction;

    std::vector<LightComponent> lights;
    std::vector<TransformComponent> light_transforms;

    uint entities_texture_id;
    uint32 selected_entity_id;
    glm::vec3 outline_color;
    float mix_factor;

    glm::vec2 viewport_size;
    uint color_texture_id;
    
    void setCommonData(uint32 current_entity_id_) {
	current_entity_id = current_entity_id_;
    }
    
    void setMeshData(MeshComponent *mesh_, glm::mat4 model_) {
	mesh = mesh_;
	model = model_;
    }
    
    void setCameraData(glm::mat4 view_, glm::mat4 projection_) {
	view = view_;
	projection = projection_;
    }
    
    void setEditorLightData(glm::vec3 view_pos_, glm::vec3 direction_) {
	view_pos = view_pos_;
	direction = direction_;
    }
    
    void setLightData(std::vector<LightComponent> lights_,
		      std::vector<TransformComponent> light_transforms_) {
	lights = lights_;
	light_transforms = light_transforms_;
    }
    
    void setOutlineData(uint color_texture_id_, uint32 entities_texture_id_,
			uint32 selected_entity_id_,
			glm::vec3 outline_color_, float mix_factor_) {
	color_texture_id = color_texture_id_;
	entities_texture_id = entities_texture_id_;
	selected_entity_id = selected_entity_id_;
	outline_color = outline_color_;
	mix_factor = mix_factor_;
    }

    void setFXAAData(glm::vec2 viewport_size_, uint color_texture_id_) {
	viewport_size = viewport_size_;
	color_texture_id = color_texture_id_;
    }
};


#endif // RENDERER_CONTEXT_H
