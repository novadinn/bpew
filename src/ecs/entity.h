#ifndef ENTITY_H
#define ENTITY_H

#include "scene.h"
#include "../core/log.h"
#include "entt/entt.hpp"

struct Entity {
	void create(entt::entity handle, Scene* new_scene) {
		entity_handle = handle;
		scene = new_scene;
	}

	template<typename T, typename... Args> T& addComponent(Args&&... args) {
		if(hasComponent<T>()) {
			LOG_ERROR("Entity already has component\n");
		}
		
		return scene->registry.emplace<T>(entity_handle, std::forward<Args>(args)...);
	}

	template<typename T> T& getComponent() {
		if(!hasComponent<T>()) {
			LOG_ERROR("Entity does not have component\n");
		}
		
		return scene->registry.get<T>(entity_handle);
	}

	template<typename T> bool hasComponent() {
		return scene->registry.all_of<T>(entity_handle);
	}

	template<typename T> void removeComponent() {
		if(!hasComponent<T>()) {
			LOG_ERROR("Entity does not have component\n");
			return;
		}
		
		scene->registry.remove<T>(entity_handle);
	}

	operator bool() const { return entity_handle != entt::null; }

private:
	entt::entity entity_handle{ entt::null };
	Scene* scene = nullptr;
};

#endif // ENTITY_H
