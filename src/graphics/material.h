#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"
#include "framebuffer.h"
#include "../nodes/node.h"

#include <vector>
#include <string>

struct Material {
  std::string name;
  std::vector<Node> nodes;
  Shader shader;
  Framebuffer preview_texture;
};

#endif // MATERIAL_H
