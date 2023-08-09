#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "../entity.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct TransformComponent {
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 scale = glm::vec3(1.0f);
  glm::vec3 rotation = glm::vec3(0.0f);

  Entity parent;

  glm::mat4 getModelMatrix();
};

#endif // TRANSFORM_COMPONENT_H
