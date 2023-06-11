#include "gizmos.h"

#include "shaders/shader.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "vertex_attribute.h"
#include "shaders/shader_builder.h"
#include "../shaders/infos/line_shader.h"

#include <vector>

static Shader line_shader;

void Gizmos::init() {    
    line_shader = ShaderBuilder::build_shader_from_create_info(line_shader_create_info);
}

void Gizmos::destroy() {
	line_shader.destroy();
}

void Gizmos::drawLine(Camera& camera, const glm::vec3& start,
		      const glm::vec3& end, const glm::vec3& color) {
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

void Gizmos::drawManupilations(ImGuizmo::OPERATION gizmo_operation,
							   float* view, float* projection,
							   float* model, bool snap) {
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y,
					  (float)ImGui::GetWindowWidth(), (float)ImGui::GetWindowHeight());

	float snap_value = 0.5f;
	if(gizmo_operation == ImGuizmo::OPERATION::ROTATE)
		snap_value = 45.0f;

	float snap_values[3] = {snap_value, snap_value, snap_value};
		
	ImGuizmo::Manipulate(view, projection,
						 (ImGuizmo::OPERATION)gizmo_operation, ImGuizmo::LOCAL,
						 model, nullptr, snap ? snap_values : nullptr);
}
