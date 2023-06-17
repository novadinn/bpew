#ifndef MATERIAL_H
#define MATERIAL_H

#include "shaders/shader.h"
#include "framebuffer.h"
#include "../nodes/node.h"
#include "shaders/shader_builder.h"

#include <vector>
#include <string>

struct Material {
  std::string name;
  std::vector<Node> nodes;
  Shader shader;
    ShaderBuilder shader_builder;
  Framebuffer preview_texture;    
};

#endif // MATERIAL_H
