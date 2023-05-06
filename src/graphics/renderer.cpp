#include "renderer.h"

#include "../graphics/shader.h"

#include <glad/glad.h>

const char* shader_vs = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	texCoord = vec2(aTexCoord.x, aTexCoord.y);
}
)";
const char* shader_fs = R"(
#version 460 core
out vec4 fragColor;

in vec2 texCoord;

uniform sampler2D mainTexture;

void main() {
	fragColor = texture(mainTexture, texCoord);
}
)";

static Shader shader;

void Renderer::init() {
	shader.createFromSource(shader_vs, shader_fs);
}

void Renderer::destroy() {
	shader.destroy();
}

void Renderer::begin(const glm::mat4& view, const glm::mat4& proj) {
	shader.bind();
	shader.setMatrix4("view", view);
	shader.setMatrix4("projection", proj);
}

void Renderer::drawMesh(const glm::mat4& model) {
	shader.setMatrix4("model", model);
}

void Renderer::end() {
	shader.unbind();
}

void Renderer::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::setClearColor(const glm::vec4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
}
