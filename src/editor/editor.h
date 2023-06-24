#ifndef EDITOR_H
#define EDITOR_H

#include "../ecs/scene.h"
#include "../ecs/entity.h"
#include "../graphics/camera.h"
#include "../graphics/framebuffer.h"
#include "../nodes/node.h"
#include "space_layout.h"
#include "space_modeling.h"
#include "space_shading.h"
#include "event_receiver.h"

#include "glm/glm.hpp"
#include <vector>

struct Editor {
    void create();
    void destroy();

    void onUpdate();
    void onDraw();

private:	
    void showMenuBar();
    void showHierarchyPanel();
    void showInspectorPanel();
    void showLines();

    template<typename T> void showAddComponentPopup(const char* str);
    template<typename T> void showRemoveComponentPopup(const char* str);

    void createNode(NodeType type);
       
    EditorContext *ctx;
        
    glm::ivec2 previous_mouse{0, 0};

    std::vector<EventReceiver *>receivers;
    EventReceiver *active_receiver;
};

#endif // EDITOR_H
