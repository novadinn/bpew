#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "../graphics/camera.h"
#include "../graphics/model.h"
#include "../graphics/shader.h"
#include "glm/glm.hpp"

#include <string>

struct TagComponent {
	std::string tag;
};

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

struct CameraComponent {
	Camera camera;
	bool main = false;
};

struct MeshComponent {
	Model model;
};

struct LightComponent {
	enum LightType {
		SPOT,
		POINT,
		DIRECTIONAL
		// TODO: area
	};
	// TODO:
	enum ShadowsType {
		NO_SHADOW,
		HARD_SHADOWS,
		SOFT_SHADOWS
	};

	LightType type;
	glm::vec3 color = {1.0f, 1.0f, 0.8f};
	float intensity = 0.5f;
	float intensity_multiplier = 0.75f;

	// Spot light properties
	float spot_angle = 30.0f;

	// Point light properies
	float range = 10.0f;

	// TODO: cookie texture
	
};

#endif // COMPONENTS_H
