#include "editor_camera.h"

void EditorCamera::create(float start_fov, float start_aspect_ratio,
                          float start_near, float start_far) {
  fov = start_fov;
  aspect_ratio = start_aspect_ratio;
  near = start_near;
  far = start_far;
}

glm::mat4 EditorCamera::getProjectionMatrix() {
  aspect_ratio = viewport_width / viewport_height;
  return glm::perspective(glm::radians(fov), aspect_ratio, near, far);
}

glm::mat4 EditorCamera::getViewMatrix() {
  position = focal_point - getForward() * distance;

  glm::quat orientation = getOrientation();
  glm::mat4 view =
      glm::translate(glm::mat4(1.0f), position) * glm::toMat4(orientation);
  view = glm::inverse(view);

  return view;
}

glm::vec3 EditorCamera::getUp() const {
  return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 EditorCamera::getRight() const {
  return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 EditorCamera::getForward() const {
  return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::quat EditorCamera::getOrientation() const {
  return glm::quat(glm::vec3(-pitch, -yaw, 0.0f));
}

glm::vec3 EditorCamera::screenToWorldDirection(glm::vec2 point) {

  glm::vec3 ndc = {(2.0f * point.x) / viewport_width - 1.0f,
                   (2.0f * point.y) / viewport_height - 1.0f, 1.0f};

  glm::vec4 clip = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);

  glm::vec4 eye = glm::inverse(getProjectionMatrix()) * clip;
  eye = glm::vec4(eye.x, eye.y, -1.0f, 0.0f);

  glm::vec3 world = glm::vec3(glm::inverse(getViewMatrix()) * eye);
  world = glm::normalize(world);

  return world;
}

void EditorCamera::pan(const glm::vec2 &delta) {
  glm::vec2 speed = panSpeed();
  focal_point += -getRight() * delta.x * speed.x * distance;
  focal_point += getUp() * delta.y * speed.y * distance;
}

void EditorCamera::rotate(const glm::vec2 &delta) {
  float yaw_sign = getUp().y < 0 ? -1.0f : 1.0f;
  yaw += yaw_sign * delta.x * rotationSpeed();
  pitch += delta.y * rotationSpeed();
}

void EditorCamera::zoom(float delta) {
  distance -= delta * zoomSpeed();
  if (distance < 1.0f) {
    focal_point += getForward();
    distance = 1.0f;
  }
}

glm::vec2 EditorCamera::panSpeed() const {
  float x = std::min(viewport_width / 1000.0f, 2.4f);
  float x_factor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

  float y = std::min(viewport_height / 1000.0f, 2.4f);
  float y_factor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

  return {x_factor, y_factor};
}

float EditorCamera::rotationSpeed() const { return 0.8f; }

float EditorCamera::zoomSpeed() {
  float dst = distance * 0.8f;
  dst = std::max(dst, 0.0f);
  float speed = dst * dst;
  speed = std::min(speed, 100.0f);

  return speed;
}
