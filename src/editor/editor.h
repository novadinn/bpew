#ifndef EDITOR_H
#define EDITOR_H

#include "../ecs/scene.h"
#include "../ecs/entity.h"
#include "../graphics/camera.h"
#include "../graphics/framebuffer.h"

#include "glm/glm.hpp"

struct Editor {

	void create();
	void destroy();

	void onUpdate();
	void onDraw();

private:
	void showViewport();
	void showMenuBar();
	void showHierarchyPanel();
	void showInspectorPanel();
	void showLines();

	template<typename T> void showAddComponentPopup(const char* str);
	template<typename T> void showRemoveComponentPopup(const char* str);
	
	Scene* scene;
	Camera* main_camera;
	Entity selected_entity;

	Framebuffer framebuffer;

	glm::ivec2 previous_mouse{0, 0};
	glm::vec2 viewport_size{1, 1};
};

#endif // EDITOR_H
