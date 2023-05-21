#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "glm/glm.hpp"

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

#endif // LIGHT_COMPONENT_H
