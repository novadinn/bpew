#include "gizmos.h"

#include "shaders/infos/overlay_grid_shader_info.h"
#include "shaders/shader.h"
#include "shaders/shader_builder.h"
#include "vertex_array.h"
#include "vertex_attribute.h"
#include "vertex_buffer.h"

#include <vector>

static Shader overlay_grid_shader;

void Gizmos::init() {
  ShaderBuilder::buildShaderFromCreateInfo(overlay_grid_shader,
                                           overlay_grid_shader_create_info);
}

void Gizmos::destroy() { overlay_grid_shader.destroy(); }

void Gizmos::drawLine(glm::mat4 &view_mat, glm::mat4 proj_mat,
                      const glm::vec3 &start, const glm::vec3 &end,
                      const glm::vec3 &color, const glm::vec3 camera_position,
                      float far) {
  glEnable(GL_BLEND);

  glLineWidth(2.0f);
  /* TODO: we dont need to create it every time */
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
  line_vertices.push_back(color.y);
  line_vertices.push_back(color.z);

  VertexArray line_va;

  line_va.create();
  line_va.bind();

  size_t vertices_size = line_vertices.size() * sizeof(float);
  std::vector<VertexAttribute> attribs = {{sizeof(float), 3, GL_FALSE},
                                          {sizeof(float), 3, GL_FALSE}};

  VertexBuffer line_vb;
  line_vb.create(line_vertices.data(), vertices_size);

  line_va.addVertexBuffer(line_vb, attribs);

  overlay_grid_shader.bind();
  line_va.bind();

  overlay_grid_shader.setMatrix4("projection", proj_mat);
  overlay_grid_shader.setMatrix4("view", view_mat);
  overlay_grid_shader.setVec3("cameraPosition", camera_position);
  overlay_grid_shader.setFloat("far", far);

  glDrawArrays(GL_LINES, 0, line_vertices.size());

  line_vb.destroy();
  line_va.destroy();

  glDisable(GL_BLEND);
}

void Gizmos::drawManupilations(ImGuizmo::OPERATION gizmo_operation, float *view,
                               float *projection, float *model, bool snap) {
  ImGuizmo::SetOrthographic(false);
  ImGuizmo::SetDrawlist();

  ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y,
                    (float)ImGui::GetWindowWidth(),
                    (float)ImGui::GetWindowHeight());

  float snap_value = 0.5f;
  if (gizmo_operation == ImGuizmo::OPERATION::ROTATE)
    snap_value = 45.0f;

  float snap_values[3] = {snap_value, snap_value, snap_value};

  ImGuizmo::Manipulate(view, projection, (ImGuizmo::OPERATION)gizmo_operation,
                       ImGuizmo::LOCAL, model, nullptr,
                       snap ? snap_values : nullptr);
}
