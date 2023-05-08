#ifndef RENDERER_H
#define RENDERER_H

#include "shader.h"
#include "model.h"
#include "camera.h"

#include <glm/glm.hpp>

struct Renderer {
	static void drawModel(Model& target, Shader shader, const Camera& camera, const glm::mat4& view, const glm::mat4& proj, const glm::mat4& model);
	
	static void clear();

	static void setClearColor(const glm::vec4& color);
};

#endif // RENDERER_H
