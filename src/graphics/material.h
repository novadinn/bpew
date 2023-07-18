#ifndef MATERIAL_H
#define MATERIAL_H

#include "../core/sha.h"
#include "../nodes/node.h"
#include "framebuffer.h"
#include "shaders/shader_builder.h"
#include "shaders/shader_container.h"

#include <vector>
#include <string>

struct Material {
  void createDefault();

  std::string name;
  std::vector<Node> nodes;
  ShaderContainer *shader_container = nullptr;
  Framebuffer preview_texture;
};

#endif // MATERIAL_H
