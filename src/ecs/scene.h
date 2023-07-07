#ifndef SCENE_H
#define SCENE_H

#include "../core/platform.h"
#include "entt/entt.hpp"

#include <glm/glm.hpp>
#include <string>

struct Entity;
struct RendererContext;

struct Scene {
    Entity createEntity(const std::string& name = std::string());
    void destroyEntity(Entity entity);
	
    void onDrawWireframe(RendererContext *context);
    void onDrawRendered(RendererContext *context);
    void onDrawSolid(RendererContext *context);
    void onDrawMaterialPreview(RendererContext *context);

    void onUpdateMaterialPreview();
    void onUpdateRendered();
    
    void onResize(uint width, uint height);

    entt::registry& getEntityRegistry() { return registry; }

private:
    entt::registry registry;
};

#endif // SCENE_H
