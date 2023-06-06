#ifndef EDITOR_H
#define EDITOR_H

#include "../ecs/scene.h"
#include "../ecs/entity.h"
#include "../graphics/camera.h"
#include "../graphics/framebuffer.h"
#include "../nodes/node.h"

#include "glm/glm.hpp"
#include <vector>

struct Editor {
  void create();
  void destroy();

  void onUpdate();
  void onDraw();

private:
  enum DrawMode {
    WIREFRAME,
    RENDERED,
    SOLID,
    MATERIAL_PREVIEW
  };
	
  void showViewSpace();
  void showNodeSpace();
  void showMenuBar();
  void showHierarchyPanel();
  void showInspectorPanel();
  void showLines();

  template<typename T> void showAddComponentPopup(const char* str);
  template<typename T> void showRemoveComponentPopup(const char* str);

  void createNode(NodeType type);

  Scene* scene;
  Camera* main_camera;
  Entity selected_entity;
  Entity hovered_entity;

  Framebuffer framebuffer;

  glm::ivec2 previous_mouse{0, 0};
  glm::vec2 viewport_size{1, 1};
  glm::vec2 viewport_bounds[2];

  bool viewport_hovered = false;

  int gizmo_operation = -1;
  DrawMode draw_mode = DrawMode::SOLID;
};

#endif // EDITOR_H
