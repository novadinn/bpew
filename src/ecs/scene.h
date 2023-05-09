#ifndef SCENE_H
#define SCENE_H

#include "../core/platform.h"
#include "entt/entt.hpp"

#include <string>

struct Entity;

struct Scene {
	Entity createEntity(const std::string& name = std::string());

	void onUpdate();
	void onResize(uint width, uint height);

private:
	entt::registry registry;

	friend struct Entity;
};

#endif // SCENE_H
