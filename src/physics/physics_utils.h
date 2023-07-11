#ifndef PHYSICS_UTILS_H
#define PHYSICS_UTILS_H

#include <glm/glm.hpp>

namespace PhysicsUtils {
    bool raySphereIntersect(glm::vec3 ray_origin, glm::vec3 ray_direction,
			    glm::vec3 sphere_position, float sphere_radius) {

	glm::vec3 oc = ray_origin - sphere_position;
	float a = glm::dot(ray_direction, ray_direction);
	float b = 2.0f * glm::dot(oc, ray_direction);
	float c = glm::dot(oc, oc) - sphere_radius * sphere_radius;
	float discriminant = b * b - 4 * a * c;

	return discriminant > 0;
    }
};

#endif // PHYSICS_UTILS_H
