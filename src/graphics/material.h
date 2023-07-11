#ifndef MATERIAL_H
#define MATERIAL_H

#include "shaders/shader_container.h"
#include "framebuffer.h"
#include "../nodes/node.h"
#include "shaders/shader_builder.h"
#include "../core/sha.h"

#include <vector>
#include <string>

struct Material {
    std::string name;
    std::vector<Node> nodes;
    ShaderContainer* shader_container = nullptr;    
    Framebuffer preview_texture;
};

#endif // MATERIAL_H
