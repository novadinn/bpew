#ifndef GLM_EXTENSIONS_H
#define GLM_EXTENSIONS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GLMExtensions {
    static glm::vec3 modelToForward(const glm::mat4& model) {
	glm::mat4 inverted = glm::inverse(model);
	glm::vec3 forward = glm::normalize(glm::vec3(inverted[2]));

	return forward;
    }
};

#endif // GLM_EXTENSIONS_H
