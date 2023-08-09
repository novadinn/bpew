#include "transform_component.h"

#include "../../core/log.h"

glm::mat4 TransformComponent::getModelMatrix() {
  glm::mat4 rot = glm::toMat4(
      glm::quat(glm::vec3(glm::radians(rotation.x), glm::radians(rotation.y),
                          glm::radians(rotation.z))));

  if (parent) {
    ASSERT(parent.hasComponent<TransformComponent>());
    auto &transform = parent.getComponent<TransformComponent>();
    return transform.getModelMatrix() *
           glm::translate(glm::mat4(1.0f), position) * rot *
           glm::scale(glm::mat4(1.0f), scale);
  }

  return glm::translate(glm::mat4(1.0f), position) * rot *
         glm::scale(glm::mat4(1.0f), scale);
}