#ifndef SHADER_INTERFACE_INFO_H
#define SHADER_INTERFACE_INFO_H

#include "../../graphics/shaders/shader_create_info.h"

const ShaderInterfaceCreateInfo vs_out = ShaderInterfaceCreateInfo::create("VS_OUT")
    .smooth(ShaderType::VEC3, "fragPos")
    .smooth(ShaderType::VEC3, "normal");

#endif
