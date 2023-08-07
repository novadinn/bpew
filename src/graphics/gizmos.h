#ifndef GIZMOS_H
#define GIZMOS_H

// clang-format off
#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
// clang-format on
#include <glm/glm.hpp>
#include <vector>

struct Gizmos {
  static void init();
  static void destroy();

  static void drawOverlayGrid(const std::vector<float> &vertices,
                              glm::mat4 &view_mat, glm::mat4 proj_mat,
                              const glm::vec3 &camera_position, float far);
  static void drawManupilations(ImGuizmo::OPERATION gizmo_operation,
                                float *view, float *projection, float *model,
                                bool snap);
};

#endif // GIZMOS_H
