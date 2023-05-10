#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "../graphics/camera.h"
#include "../graphics/model.h"
#include "../graphics/shader.h"
#include "glm/glm.hpp"

#include <string>

struct TagComponent {
	TagComponent() = default;
	TagComponent(std::string name) : tag(name) {}
	
	std::string tag;
};

struct TransformComponent {
	TransformComponent() = default;
	TransformComponent(glm::vec3 pos, glm::vec3 sca, glm::vec3 rot) :
		position(pos), scale(sca), rotation(rot) {}
	
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
	CameraComponent() = default;
	CameraComponent(Camera cam) : camera(cam) {}
	
	Camera camera;
	bool main = false;
};

struct MeshComponent {
	MeshComponent() = default;
	MeshComponent(Model mod, Shader sha) : model(mod), shader(sha) {}
	
	Model model;
	Shader shader;
};

#endif // COMPONENTS_H
