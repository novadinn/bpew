#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

struct Camera {

	void create(float fov, float aspect_ratio, float near, float far);

	inline void setViewportSize(float width, float height) {
		viewport_width = width;
		viewport_height = height;
	}
	
	glm::mat4 getProjectionMatrix();
	glm::mat4 getViewMatrix();

	glm::vec3 getUp() const;
	glm::vec3 getRight() const;
	glm::vec3 getForward() const;
	const glm::vec3& getPosition() const { return position; }
	glm::quat getOrientation() const;

	float getDistance() const { return distance; }
	float getPitch() const { return pitch; }
	float getYaw() const { return yaw; }

	void pan(const glm::vec2& delta);
	void rotate(const glm::vec2& delta);
	void zoom(float delta);
	
private:
	glm::vec2 panSpeed() const;
	float rotationSpeed() const;
	float zoomSpeed();

	float fov = 45.0f, aspect_ratio = 1.778f, near = 0.1f, far = 1000.0f;

	glm::vec3 position = {0, 0, 0};
	glm::vec3 focal_point = {0, 0, 0};

	float distance = 10.0f;
	float pitch = 0.0f, yaw = 0.0f;

	float viewport_width = 1280, viewport_height = 720;
};

#endif // CAMERA_H
