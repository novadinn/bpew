#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>

struct Renderer {
	static void init();
	static void destroy();

	static void begin(const glm::mat4& view, const glm::mat4& proj);
	// TODO: this should take a mesh
	static void drawMesh(const glm::mat4& model);
	static void end();

	static void clear();

	static void setClearColor(const glm::vec4& color);
};

#endif // RENDERER_H
