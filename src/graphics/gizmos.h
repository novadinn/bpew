#ifndef GIZMOS_H
#define GIZMOS_H

#include "ImGuizmo/ImGuizmo.h"
#include "imgui/imgui.h"
#include <glm/glm.hpp>

struct Gizmos {
  static void init();
  static void destroy();

  static void drawLine(glm::mat4 &view_mat, glm::mat4 proj_mat,
                       const glm::vec3 &start, const glm::vec3 &end,
                       const glm::vec3 &color);
  static void drawManupilations(ImGuizmo::OPERATION gizmo_operation,
                                float *view, float *projection, float *model,
                                bool snap);
};

#endif // GIZMOS_H
