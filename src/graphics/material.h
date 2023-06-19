#ifndef MATERIAL_H
#define MATERIAL_H

#include "shaders/shader.h"
#include "framebuffer.h"
#include "../nodes/node.h"
#include "shaders/shader_builder.h"
#include "../core/sha.h"

#include <vector>
#include <string>

struct Material {
    std::string name;
    std::vector<Node> nodes;
    Shader shader;    
    Framebuffer preview_texture;
    Sha sha;
};

#endif // MATERIAL_H
