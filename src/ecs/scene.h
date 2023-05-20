#ifndef SCENE_H
#define SCENE_H

#include "../core/platform.h"
#include "entt/entt.hpp"

#include <string>

struct Entity;

struct Scene {
	Entity createEntity(const std::string& name = std::string());
	void destroyEntity(Entity entity);
	
	void onDrawWireframe();
	void onDrawRendered();
	void onDrawSolid();
	void onDrawMaterialPreview();
	
	void onResize(uint width, uint height);

	entt::registry& getEntityRegistry() { return registry; }

private:
	entt::registry registry;
};

#endif // SCENE_H
