#ifndef MATERIAL_SHADER_H
#define MATERIAL_SHADER_H
#include "../../graphics/shaders/shader_create_info.h"

#include "info_defines.h"

const ShaderCreateInfo material_shader_create_info = ShaderCreateInfo::create()
    .vin(ShaderType::VEC3, 0, "aPos")
    .vin(ShaderType::VEC3, 1, "aNormal")
    .vin(ShaderType::VEC2, 2, "aTexCoord")
    .vin(ShaderType::VEC3, 3, "aTangent")
    .vin(ShaderType::VEC3, 4, "aBitangent")    
    .fout(ShaderType::VEC4, 0, "fragColor")
    .fout(ShaderType::VEC4, 1, "entityID")
    .interface("VS_OUT", "vs_in", "vs_out", VS_OUT)    
    .uniform(ShaderType::MAT4, "model")
    .uniform(ShaderType::MAT4, "view")
    .uniform(ShaderType::MAT4, "projection")    
    .uniform("DirLight", "dirLight")
    .uniform(ShaderType::FLOAT, "shininess")
    .uniform(ShaderType::INT, "entity")
    .uniform(ShaderType::VEC3, "viewPos")
    .dep("lights_lib.glsl")
    .vertex("material_shader_vs.glsl")
    .fragment("material_shader_fs.glsl");		    

#endif
