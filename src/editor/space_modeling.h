#ifndef SPACE_MODELING_H
#define SPACE_MODELING_H

#include "../ecs/components/transform_component.h"

#include <glm/glm.hpp>

struct SpaceModeling {
    void begin();
    
    void drawImage(uint texture_attachment_id, const glm::vec2& viewport_size);
    void manipulateSelected(TransformComponent& transform,
			    glm::mat4& view, glm::mat4& projection,
			    int gizmo_operation, bool snap);

    void end();
};

#endif // SPACE_MODELING_H