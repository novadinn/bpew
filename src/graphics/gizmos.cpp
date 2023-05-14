#include "gizmos.h"

#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "vertex_attribute.h"

#include <vector>

const char* line_shader_vs = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 color;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0f);
    color = aColor;
}
)";
const char* line_shader_fs = R"(
#version 460 core

out vec4 fragColor;

in vec3 color;

void main() {
    fragColor = vec4(color, 1.0);
}
)";

static Shader line_shader;

void Gizmos::init() {
	line_shader.createFromSource(line_shader_vs, line_shader_fs);
}

void Gizmos::destroy() {
	line_shader.destroy();
}

void Gizmos::drawLine(Camera& camera,
					  const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
	// TODO: we dont need to create it every time
	std::vector<float> line_vertices;
	line_vertices.reserve(12);
	
	line_vertices.push_back(start.x);
	line_vertices.push_back(start.y);
	line_vertices.push_back(start.z);
	
	line_vertices.push_back(color.x);
	line_vertices.push_back(color.y);
	line_vertices.push_back(color.z);
	
	line_vertices.push_back(end.x);
	line_vertices.push_back(end.y);
	line_vertices.push_back(end.z);
	
	line_vertices.push_back(color.x);
	line_vertices.push_back( color.y);
	line_vertices.push_back( color.z);
	
	VertexArray line_va;
	
	line_va.create();
	line_va.bind();
	
	size_t vertices_size = line_vertices.size() * sizeof(float);
	std::vector<VertexAttribute> attribs = {
		{sizeof(float), 3, GL_FALSE},
		{sizeof(float), 3, GL_FALSE}
	};

	VertexBuffer line_vb;
	line_vb.create(line_vertices.data(), vertices_size);

	line_va.addVertexBuffer(line_vb, attribs);
	
	line_shader.bind();
	line_va.bind();

	line_shader.setMatrix4("projection", camera.getProjectionMatrix());
	line_shader.setMatrix4("view", camera.getViewMatrix());

	glDrawArrays(GL_LINES, 0, line_vertices.size());

	line_vb.destroy();
	line_va.destroy();
}
