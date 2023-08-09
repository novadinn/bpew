#include "gizmos.h"

#include "batch.h"
#include "shaders/infos/overlay_grid_shader_info.h"
#include "shaders/shader.h"
#include "shaders/shader_builder.h"
#include "vertex_array.h"
#include "vertex_attribute.h"
#include "vertex_buffer.h"

#include <vector>

static Shader overlay_grid_shader;
static Batch overlay_grid_batch;
static int overlay_grid_batch_capacity = 0;

void Gizmos::init() {
  ShaderBuilder::buildShaderFromCreateInfo(overlay_grid_shader,
                                           overlay_grid_shader_create_info);

  const std::vector<VertexAttribute> attribs = {{sizeof(float), 3, GL_FALSE},
                                                {sizeof(float), 3, GL_FALSE}};
  overlay_grid_batch.create(overlay_grid_batch_capacity, attribs);
}

void Gizmos::destroy() {
  overlay_grid_shader.destroy();
  overlay_grid_batch.destroy();
}

void Gizmos::drawOverlayGrid(const std::vector<float> &vertices,
                             glm::mat4 &view_mat, glm::mat4 proj_mat,
                             const glm::vec3 &camera_position, float far) {
  glEnable(GL_BLEND);

  glLineWidth(2.0f);

  if (vertices.size() * sizeof(float) > overlay_grid_batch_capacity) {
    std::vector<VertexAttribute> attribs = {{sizeof(float), 3, GL_FALSE},
                                            {sizeof(float), 3, GL_FALSE}};

    overlay_grid_batch_capacity = vertices.size() * sizeof(float);
    overlay_grid_batch.destroy();
    overlay_grid_batch.create(overlay_grid_batch_capacity, attribs);
  }

  overlay_grid_batch.flush(vertices);

  overlay_grid_shader.bind();
  overlay_grid_batch.va.bind();

  overlay_grid_shader.setMatrix4("projection", proj_mat);
  overlay_grid_shader.setMatrix4("view", view_mat);
  overlay_grid_shader.setVec3("cameraPosition", camera_position);
  overlay_grid_shader.setFloat("far", far);

  glDrawArrays(GL_LINES, 0, vertices.size());

  overlay_grid_batch.va.unbind();

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
