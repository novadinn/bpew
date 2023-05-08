#include "renderer.h"

#include "../graphics/shader.h"

#include <glad/glad.h>

void Renderer::drawModel(Model& target, Shader shader, const Camera& camera,
						 const glm::mat4& view, const glm::mat4& proj, const glm::mat4& model) {
	for(int i = 0; i < target.meshes.size(); ++i) {
		shader.bind();
		shader.setMatrix4("view", view);
		shader.setMatrix4("projection", proj);
		shader.setMatrix4("model", model);

		shader.setFloat("shininess", 0.2f);
		shader.setVec3("viewPos", camera.getPosition());
		shader.setVec3("dirLight.direction", camera.getForward());
		shader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		shader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		shader.setVec3("dirLight.specilar", glm::vec3(0.5f, 0.5f, 0.5f));
	
		Mesh& mesh = target.meshes[i];
		
		mesh.va.bind();
		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
		mesh.va.unbind();

		shader.unbind();
	}
}

void Renderer::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setClearColor(const glm::vec4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
}
