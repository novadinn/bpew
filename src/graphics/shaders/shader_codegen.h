#ifndef SHADER_CODEGEN_H
#define SHADER_CODEGEN_H

#include <sstream>

#include "../graphics/material.h"
#include "shader_create_info.h"

struct ShaderCodegen {
    ShaderCreateInfo create_material_info(Material& material);
private:
    ShaderCreateInfo info;	
};

#endif
