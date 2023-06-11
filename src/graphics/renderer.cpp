#include "renderer.h"

#include "../graphics/shaders/shader.h"
#include "../core/glm_extensions.h"
#include "../core/log.h"
#include "shaders/shader_builder.h"
#include "../shaders/infos/solid_shader.h"

#include <glad/glad.h>

const char* render_shader_vs = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 worldPosition = model* vec4(aPos, 1.0f);

    vs_out.fragPos = vec3(worldPosition);
    vs_out.normal = mat3(transpose(inverse(model))) * aNormal;

	gl_Position = projection * view * worldPosition;
}
)";
const char* render_shader_fs = R"(
#version 460 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out int entityID;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float intensity;
    float intensity_multiplier;
    vec3 color;
    float spot_angle;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float intensity_multiplier;
    float range;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
    float intensity_multiplier;
};

// TODO: set those dynamically
#define NUM_SPOT_LIGHTS 16
#define NUM_POINT_LIGHTS 16
#define NUM_DIRECTIONAL_LIGHTS 16

uniform vec3 viewPos;
uniform float shininess;
uniform SpotLight spotLights[NUM_SPOT_LIGHTS];
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform DirectionalLight dirLights[NUM_DIRECTIONAL_LIGHTS];
uniform int entity;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
} vs_in;

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);

void main() {
    vec3 norm = normalize(vs_in.normal);
    vec3 viewDir = normalize(viewPos - vs_in.fragPos);

    vec3 result;
    for(int i = 0; i < NUM_SPOT_LIGHTS; i++)
        result += calcSpotLight(spotLights[i], norm, vs_in.fragPos, viewDir);
    for(int i = 0; i < NUM_POINT_LIGHTS; i++)
        result += calcPointLight(pointLights[i], norm, vs_in.fragPos, viewDir);
    for(int i = 0; i < NUM_DIRECTIONAL_LIGHTS; i++)
        result += calcDirectionalLight(dirLights[i], norm, viewDir);

	fragColor = vec4(0.5f, 0.5f, 0.5f, 1.0f) + vec4(result, 0.0f);
    entityID = entity;
}

// TODO: add specular highlight
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    // TODO: these are wrong calculations
    vec3 light_direction = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
    float diffuse_intensity = max(dot(normal, light_direction), 0.0);
    float spot_factor = dot(-light_direction, normalize(light.direction));
    float spot_attenuation = smoothstep(light.spot_angle, light.spot_angle * 0.9, spot_factor);

    return light.color * diffuse_intensity * attenuation * spot_attenuation * light.intensity * light.intensity_multiplier;
}

// TODO: add specular highlight
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 light_direction = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    // NOTE: from float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
    float diffuse_intensity = max(dot(normal, light_direction), 0.0);
    float falloff = smoothstep(light.range, light.range * 0.9, distance);

    return light.color * diffuse_intensity * attenuation * falloff * light.intensity * light.intensity_multiplier;
}

// TODO: add specular highlight
vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 light_direction = normalize(-light.direction);
    float diffuse_intensity = max(dot(normal, light_direction), 0.0);

    return light.color * light.intensity * diffuse_intensity * light.intensity_multiplier;
}
)";

static Shader solid_shader;
static Shader render_shader;

void Renderer::init() {
    solid_shader = ShaderBuilder::build_shader_from_create_info(solid_shader_create_info);
    render_shader.createFromSource(render_shader_vs, render_shader_fs);
}

void Renderer::destroy() {
    solid_shader.destroy();
}

void Renderer::drawMesh(uint32 entity_id, MeshComponent& mesh, CameraComponent& camera,
			TransformComponent& camera_tr,
			std::vector<LightComponent>& lights,
			std::vector<TransformComponent>& light_transforms,
			const glm::mat4& model) {

    // TODO:
}

void Renderer::drawMeshSolid(uint32 entity_id, MeshComponent& mesh, CameraComponent& camera,
			     TransformComponent& camera_tr, const glm::mat4& model) {
    for(int i = 0; i < mesh.model.meshes.size(); ++i) {
	solid_shader.bind();
	solid_shader.setMatrix4("view", camera.camera.getViewMatrix());
	solid_shader.setMatrix4("projection", camera.camera.getProjectionMatrix());
	solid_shader.setMatrix4("model", model);

	solid_shader.setFloat("shininess", 0.2f);
	solid_shader.setVec3("viewPos", camera_tr.position);
	solid_shader.setVec3("dirLight.direction", camera.camera.getForward());
	solid_shader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	solid_shader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	solid_shader.setVec3("dirLight.specilar", glm::vec3(0.5f, 0.5f, 0.5f));
	solid_shader.setInt("entity", (int)entity_id);
		
	Mesh& target = mesh.model.meshes[i];
		
	target.va.bind();
	glDrawElements(GL_TRIANGLES, target.indices.size(), GL_UNSIGNED_INT, 0);
	target.va.unbind();

	solid_shader.unbind();
    }
}

void Renderer::drawMeshRendered(uint32 entity_id, MeshComponent& mesh, CameraComponent& camera,
				TransformComponent& camera_tr,
				std::vector<LightComponent>& lights,
				std::vector<TransformComponent>& light_transforms,
				const glm::mat4& model) {
    if(lights.size() != light_transforms.size()) {
	LOG_ERROR("Provided lights and light transforms sizes does not match\n");
	return;
    }

    for(int i = 0; i < mesh.model.meshes.size(); ++i) {
	render_shader.bind();
	render_shader.setMatrix4("view", camera.camera.getViewMatrix());
	render_shader.setMatrix4("projection", camera.camera.getProjectionMatrix());
	render_shader.setMatrix4("model", model);

	render_shader.setVec3("viewPos", camera_tr.position);
	render_shader.setFloat("shininess", 0.2f);
	render_shader.setInt("entity", (int)entity_id);
		
	// Refresh the shader values so that it will not affect the shader call
	// TODO: sizes may not match, we need to query it smh
	for(int j = 0; j < 16; ++j) {
	    std::string element = "spotLights[" + std::to_string(j) + "]";
	    std::string value = element + ".intensity_multiplier";
	    render_shader.setFloat(value.c_str(), 0);

	    element = "pointLights[" + std::to_string(j) + "]";
	    value = element + ".intensity_multiplier";
	    render_shader.setFloat(value.c_str(), 0);

	    element = "dirLights[" + std::to_string(j) + "]";
	    value = element + ".intensity_multiplier";
	    render_shader.setFloat(value.c_str(), 0);
	}
		
	int num_spot_lights = 0;
	int num_point_lights = 0;
	int num_dir_lights = 0;
	for(int j = 0; j < lights.size(); ++j) {
	    LightComponent::LightType type = lights[j].type;
	    switch(type) {
	    case LightComponent::LightType::SPOT: {
		std::string element = "spotLights[" + std::to_string(num_spot_lights++) + "]";
				
		std::string value = element + ".position";				
		render_shader.setVec3(value.c_str(), light_transforms[j].position);
		value = element + ".direction";
		render_shader.setVec3(value.c_str(), GLMExtensions::modelToForward(light_transforms[j].getModelMatrix()));
		value = element + ".intensity";
		render_shader.setFloat(value.c_str(), lights[j].intensity);
		value = element + ".intensity_multiplier";
		render_shader.setFloat(value.c_str(), lights[j].intensity_multiplier);
		value = element + ".color";
		render_shader.setVec3(value.c_str(), lights[j].color);
		value = element + ".spot_angle";
		render_shader.setFloat(value.c_str(), lights[j].spot_angle);
				
	    } break;
	    case LightComponent::LightType::POINT: {
		std::string element = "pointLights[" + std::to_string(num_point_lights++) + "]";
				
		std::string value = element + ".position";
		render_shader.setVec3(value.c_str(), light_transforms[j].position);
		value = element + ".color";
		render_shader.setVec3(value.c_str(), lights[j].color);
		value = element + ".intensity";
		render_shader.setFloat(value.c_str(), lights[j].intensity);
		value = element + ".intensity_multiplier";
		render_shader.setFloat(value.c_str(), lights[j].intensity_multiplier);
		value = element + ".range";
		render_shader.setFloat(value.c_str(), lights[j].range);
	    } break;
	    case LightComponent::LightType::DIRECTIONAL: {
		std::string element = "dirLights[" + std::to_string(num_dir_lights++) + "]";
				
		std::string value = element + ".direction";
		render_shader.setVec3(value.c_str(), GLMExtensions::modelToForward(light_transforms[j].getModelMatrix()));
		value = element + ".color";
		render_shader.setVec3(value.c_str(), lights[j].color);
		value = element + ".intensity";
		render_shader.setFloat(value.c_str(), lights[j].intensity);
		value = element + ".intensity_multiplier";
		render_shader.setFloat(value.c_str(), lights[j].intensity_multiplier);
	    } break;
	    };
	}
	
	Mesh& target = mesh.model.meshes[i];
		
	target.va.bind();
	glDrawElements(GL_TRIANGLES, target.indices.size(), GL_UNSIGNED_INT, 0);
	target.va.unbind();

	render_shader.unbind();
    }
}

void Renderer::drawMeshWireframe(uint32 entity_id, MeshComponent& mesh, CameraComponent& camera,
				 TransformComponent& camera_tr,
				 const glm::mat4& model) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawMeshSolid(entity_id, mesh, camera, camera_tr, model);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setClearColor(const glm::vec4& color) {
    glClearColor(color.x, color.y, color.z, color.w);
}
