#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

struct CameraComponent {
  glm::mat4 getProjectionMatrix() {
    aspect_ratio = viewport_width / viewport_height;
    return glm::perspective(glm::radians(fov), aspect_ratio, near, far);
  }

  glm::mat4 getViewMatrix(glm::vec3 &position, glm::vec3 &rotation) {
    glm::vec3 pos = position + (focal_point - getForward(rotation) * distance);

    glm::quat orientation = getOrientation(rotation);
    glm::mat4 view =
        glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(orientation);
    view = glm::inverse(view);

    return view;
  }

  glm::vec3 getForward(glm::vec3 &rotation) const {
    return glm::rotate(getOrientation(rotation), glm::vec3(0.0f, 0.0f, -1.0f));
  }

  glm::quat getOrientation(glm::vec3 &rotation) const {
    return glm::quat(glm::vec3(-rotation.x, -rotation.y, 0.0f));
  }

  float zoomSpeed() {
    float dst = distance * 0.8f;
    dst = std::max(dst, 0.0f);
    float speed = dst * dst;
    speed = std::min(speed, 100.0f);

    return speed;
  }

  void zoom(float delta, glm::vec3 &rotation) {
    distance -= delta * zoomSpeed();
    if (distance < 1.0f) {
      focal_point += getForward(rotation);
      distance = 1.0f;
    }
  }

  void setViewportSize(float width, float height) {
    viewport_width = width;
    viewport_height = height;
  }

  float fov = 45.0f, near = 0.1f, far = 1000.0f;

private:
  glm::vec3 focal_point = {0, 0, 0};
  float aspect_ratio = 1.778f;
  float distance = 10.0f;
  float viewport_width = 1280, viewport_height = 720;
};

#endif // CAMERA_COMPONENT_H
