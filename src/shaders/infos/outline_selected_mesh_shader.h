#ifndef OUTLINE_SELECTED_MESH_SHADER_H
#define OUTLINE_SELECTED_MESH_SHADER_H

#include "../../graphics/shaders/shader_create_info.h"

const ShaderCreateInfo outline_selected_mesh_shader_create_info = ShaderCreateInfo::create()
    .vin(ShaderType::VEC2, 0, "aPos")
    .vin(ShaderType::VEC2, 1, "aTexCoord")
    .vout("VS_OUT", "vs_inout")
    .fin("VS_OUT", "vs_inout")
    .fout(ShaderType::VEC4, 0, "fragColor")
    .uniform(ShaderType::SAMPLER_2D, "colorTexture")
    .uniform(ShaderType::SAMPLER_2D, "entitiesTexture")
    .uniform(ShaderType::INT, "selectedEntityID")
    .uniform(ShaderType::VEC3, "outlineColor")
    .uniform(ShaderType::FLOAT, "mixFactor")
    .typedep("outline_selected_mesh_shader_defines.glsl")    
    .vertex("outline_selected_mesh_vs.glsl")
    .fragment("outline_selected_mesh_fs.glsl");

#endif
