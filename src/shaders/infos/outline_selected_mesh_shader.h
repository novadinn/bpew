#ifndef OUTLINE_SELECTED_MESH_SHADER_H
#define OUTLINE_SELECTED_MESH_SHADER_H

#include "../../graphics/shaders/shader_create_info.h"

const ShaderInterfaceCreateInfo outline_vs_out = ShaderInterfaceCreateInfo::create("VS_OUT")
    .smooth(ShaderType::VEC2, "texCoord")
    .noperspective(ShaderType::VEC2, "texCoordNoPInterp");

const ShaderCreateInfo outline_selected_mesh_shader_create_info = ShaderCreateInfo::create()
    .vin(ShaderType::VEC2, 0, "aPos")
    .vin(ShaderType::VEC2, 1, "aTexCoord")
    .interface(outline_vs_out, "vs_inout")
    .fout(ShaderType::VEC4, 0, "fragColor")
    .uniform(ShaderType::SAMPLER_2D, "colorTexture")
    .uniform(ShaderType::SAMPLER_2D, "entitiesTexture")
    .uniform(ShaderType::INT, "selectedEntityID")
    .uniform(ShaderType::VEC3, "outlineColor")
    .uniform(ShaderType::VEC2, "viewportSize")
    .uniform(ShaderType::FLOAT, "mixFactor")    
    .dep("fxaa_lib.glsl")
    .vertex("outline_selected_mesh_vs.glsl")
    .fragment("outline_selected_mesh_fs.glsl");

#endif
