#include "renderer.h"

#include "../graphics/shaders/shader.h"
#include "../core/glm_extensions.h"
#include "../core/log.h"
#include "shaders/shader_builder.h"
#include "../shaders/infos/solid_shader.h"

#include <glad/glad.h>

static Shader solid_shader;
static Shader outline_selected_mesh_shader;


float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};
unsigned int quadVAO, quadVBO;


void Renderer::init() {
    ShaderBuilder::buildShaderFromCreateInfo(solid_shader, solid_shader_create_info);  
  outline_selected_mesh_shader.createFromFile("shaders/outline_selected_mesh_vert.glsl",
			       "shaders/outline_selected_mesh_frag.glsl");

  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Renderer::destroy() {
  solid_shader.destroy();
  outline_selected_mesh_shader.destroy();
}

void Renderer::drawMeshMaterial(uint32 entity_id, MeshComponent& mesh, CameraComponent& camera,
				TransformComponent& camera_transform, const glm::mat4& model) {    
    Material* material = mesh.getActiveMaterial();
    if(!material) {
	drawMeshSolid(entity_id, mesh, camera, camera_transform, model);
	return;
    }

    // TODO: each mesh has its own material
    for(int i = 0; i < mesh.model.meshes.size(); ++i) {
	Mesh& target = mesh.model.meshes[i];
	
	material->shader.bind();
	material->shader.setMatrix4("view", camera.camera.getViewMatrix());
	material->shader.setMatrix4("projection", camera.camera.getProjectionMatrix());
	material->shader.setMatrix4("model", model);

	material->shader.setFloat("shininess", 0.2f);
	material->shader.setVec3("viewPos", camera_transform.position);
	material->shader.setVec3("dirLight.direction", camera.camera.getForward());
	material->shader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	material->shader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	material->shader.setVec3("dirLight.specilar", glm::vec3(0.5f, 0.5f, 0.5f));
	material->shader.setInt("entity", (int)entity_id);

	for(auto& node : material->nodes) {
	    setMaterialNodeUniforms(material->shader, node);
	}

	target.va.bind();
	glDrawElements(GL_TRIANGLES, target.indices.size(), GL_UNSIGNED_INT, 0);
	target.va.unbind();
	
	material->shader.unbind();
    }
}

void Renderer::drawMeshSolid(uint32 entity_id, MeshComponent& mesh, CameraComponent& camera,
			     TransformComponent& camera_transform,
			     const glm::mat4& model) {
  for(int i = 0; i < mesh.model.meshes.size(); ++i) {
    solid_shader.bind();
    solid_shader.setMatrix4("view", camera.camera.getViewMatrix());
    solid_shader.setMatrix4("projection", camera.camera.getProjectionMatrix());
    solid_shader.setMatrix4("model", model);

    solid_shader.setFloat("shininess", 0.2f);
    solid_shader.setVec3("viewPos", camera_transform.position);
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
				std::vector<LightComponent>& lights,
				std::vector<TransformComponent>& light_transforms,
				const glm::mat4& model) {
  // TODO: 
}

void Renderer::drawMeshWireframe(uint32 entity_id, MeshComponent& mesh, CameraComponent& camera,
				 TransformComponent& camera_transform, const glm::mat4& model) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  drawMeshSolid(entity_id, mesh, camera, camera_transform, model);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::outlineSelectedMesh(uint color_texture_id, uint entities_texture_id,
				    uint32 selected_entity_id, glm::vec3 outline_color,
				    float mix_factor) {

    outline_selected_mesh_shader.bind();

    outline_selected_mesh_shader.setInt("colorTexture", 0);
    outline_selected_mesh_shader.setInt("entitiesTexture", 1);
    outline_selected_mesh_shader.setInt("selectedEntityID", selected_entity_id);
    outline_selected_mesh_shader.setVec3("outlineColor", outline_color);
    outline_selected_mesh_shader.setFloat("mixFactor", mix_factor);

    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_texture_id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, entities_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    
    outline_selected_mesh_shader.unbind();
}

void Renderer::clear() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setClearColor(const glm::vec4& color) {
  glClearColor(color.x, color.y, color.z, color.w);
}

void Renderer::setMaterialNodeUniforms(Shader& shader, Node& node) {    
    for(auto& input : node.inputs) {
	// check if should be uniform
	if(input.links.size() == 0) {	    	   	   
	    setMaterialNodeUniform(shader, node, input);
	}
    }    
}

void Renderer::setMaterialNodeUniform(Shader& shader, Node& node, NodeProperty& prop) {    
    std::string name = std::string("input_") + std::to_string(node.id.id) + std::string("_") + std::to_string(prop.id.id);    
    switch(prop.type) {
    case NodePropertyType::COLOR:
	shader.setVec4(name.c_str(), prop.value.color_value);
	break;
    case NodePropertyType::VECTOR3:
	shader.setVec3(name.c_str(), prop.value.vector3_value);
	break;
    case NodePropertyType::VECTOR2:
	shader.setVec2(name.c_str(), prop.value.vector2_value);
	break;
    case NodePropertyType::FLOAT:
	shader.setFloat(name.c_str(), prop.value.float_value);
	break;
    case NodePropertyType::INT:
	shader.setInt(name.c_str(), prop.value.int_value);
	break;
    case NodePropertyType::ENUM:
	shader.setInt(name.c_str(), prop.value.enum_value);
	break;
    case NodePropertyType::SHADER:
	// TODO: what we should do?
	break;
    case NodePropertyType::TEXTURE:
	shader.setInt(name.c_str(), prop.value.texture_value.getID());
	break;
    default:
	printf("Unhandled node property type\n");
    }
}
