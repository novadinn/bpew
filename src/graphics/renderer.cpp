#include "renderer.h"

#include "../graphics/shader.h"
#include "../core/glm_extensions.h"
#include "../core/log.h"

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
  solid_shader.createFromFile("shaders/solid_vert.glsl", "shaders/solid_frag.glsl");
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
				std::vector<LightComponent>& lights,
				std::vector<TransformComponent>& light_transforms,
				const glm::mat4& model) {

  // TODO:
}

void Renderer::drawMeshSolid(uint32 entity_id, MeshComponent& mesh, CameraComponent& camera,
			     const glm::mat4& model) {
  for(int i = 0; i < mesh.model.meshes.size(); ++i) {
    solid_shader.bind();
    solid_shader.setMatrix4("view", camera.camera.getViewMatrix());
    solid_shader.setMatrix4("projection", camera.camera.getProjectionMatrix());
    solid_shader.setMatrix4("model", model);

    solid_shader.setFloat("shininess", 0.2f);
    solid_shader.setVec3("viewPos", camera.camera.getPosition());
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
				 const glm::mat4& model) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  drawMeshSolid(entity_id, mesh, camera, model);
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
