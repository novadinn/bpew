#ifndef SHADER_CONTAINER_H
#define SHADER_CONTAINER_H

#include "../../core/sha.h"
#include "shader.h"
#include "shader_create_info.h"

struct ShaderContainer {
  Sha sha;
  uint users = 0;

  Shader shader;

  bool compiled = false;

  ShaderCreateInfo create_info;

  std::string material_functions;
};

#endif
