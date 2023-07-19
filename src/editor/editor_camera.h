#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

struct EditorCamera {
  void create(float fov, float aspect_ratio, float near, float far);

  inline void setViewportSize(float width, float height) {
    viewport_width = width;
    viewport_height = height;
  }

  glm::mat4 getProjectionMatrix();
  glm::mat4 getViewMatrix();
  glm::vec3 getUp() const;
  glm::vec3 getRight() const;
  glm::vec3 getForward() const;
  glm::quat getOrientation() const;

  glm::vec3 screenToWorldDirection(glm::vec2 point);

  float getDistance() const { return distance; }

  void pan(const glm::vec2 &delta);
  void rotate(const glm::vec2 &delta);
  void zoom(float delta);

  float fov = 45.0f, near = 0.1f, far = 1000.0f;
  /* TODO: remove this var, since it is always accessible from the view matrix
   */
  glm::vec3 position;
  bool orthographic = false;
  float pitch = 0.0f, yaw = 0.0f;
  glm::vec3 focal_point = {0, 0, 0};
  float aspect_ratio = 1.778f;
  float distance = 10.0f;
  float viewport_width = 1280, viewport_height = 720;

private:
  glm::vec2 panSpeed() const;
  float rotationSpeed() const;
  float zoomSpeed();
};

#endif
