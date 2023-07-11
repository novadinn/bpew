#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent {
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::vec3 rotation = glm::vec3(0.0f);

	glm::mat4 getModelMatrix() const {
	    glm::mat4 rot = glm::toMat4(glm::quat(glm::vec3(
												  glm::radians(rotation.x),
												  glm::radians(rotation.y),
												  glm::radians(rotation.z))));

	    return glm::translate(glm::mat4(1.0f), position)
			* rot
			* glm::scale(glm::mat4(1.0f), scale);
	}
};

#endif // TRANSFORM_COMPONENT_H
