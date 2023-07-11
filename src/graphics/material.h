#ifndef MATERIAL_H
#define MATERIAL_H

#include "../core/sha.h"
#include "../nodes/node.h"
#include "framebuffer.h"
#include "shaders/shader_builder.h"
#include "shaders/shader_container.h"

#include <string>
#include <vector>

struct Material {
  std::string name;
  std::vector<Node> nodes;
  ShaderContainer *shader_container = nullptr;
  Framebuffer preview_texture;
};

#endif // MATERIAL_H
