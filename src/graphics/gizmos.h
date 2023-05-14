#ifndef GIZMOS_H
#define GIZMOS_H

#include "camera.h"

#include <glm/glm.hpp>

struct Gizmos {
	static void init();
	static void destroy();
	static void drawLine(Camera& camera, const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
};

#endif // GIZMOS_H
