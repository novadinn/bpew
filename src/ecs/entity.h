#ifndef ENTITY_H
#define ENTITY_H

#include "scene.h"
#include "../core/log.h"
#include "../core/platform.h"
#include "entt/entt.hpp"

struct Entity {
    void create(entt::entity handle, Scene* new_scene) {
	entity_handle = handle;
	scene = new_scene;
    }

    template<typename T> T& addComponent() {
	if(hasComponent<T>()) {
	    LOG_ERROR("Entity already has component\n");
	}
		
	return scene->getEntityRegistry().emplace<T>(entity_handle);
    }

    template<typename T> T& getComponent() {
	if(!hasComponent<T>()) {
	    LOG_ERROR("Entity does not have component\n");
	}
		
	return scene->getEntityRegistry().get<T>(entity_handle);
    }

    template<typename T> bool hasComponent() {
	return scene->getEntityRegistry().all_of<T>(entity_handle);
    }

    template<typename T> void removeComponent() {
	if(!hasComponent<T>()) {
	    LOG_ERROR("Entity does not have component\n");
	    return;
	}
		
	scene->getEntityRegistry().remove<T>(entity_handle);
    }

    operator bool() const { return entity_handle != entt::null; }
    operator entt::entity() const { return entity_handle; }
    operator uint32() const { return (uint32)entity_handle; }
    bool operator==(const Entity& other) const {
	return entity_handle == other.entity_handle;
    }
    bool operator!=(const Entity& other) const {
	return !((*this) == other);
    }
    bool operator==(const entt::entity& other) const {
	return entity_handle == other;
    }
    bool operator!=(const entt::entity& other) const {
	return !((*this) == other);
    }
	
private:
    entt::entity entity_handle{ entt::null };
    Scene* scene = nullptr;
};

#endif // ENTITY_H
