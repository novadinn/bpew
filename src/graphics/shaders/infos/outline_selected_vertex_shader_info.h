#ifndef OUTLINE_SELECTED_VERTEX_SHADER_INFO_H
#define OUTLINE_SELECTED_VERTEX_SHADER_INFO_H

#include "../shader_create_info.h"

const ShaderInterfaceCreateInfo outline_selected_vertex_vs_out =
    ShaderInterfaceCreateInfo::create("VS_OUT").smooth(ShaderType::VEC2,
                                                       "texCoord");

const ShaderCreateInfo outline_selected_vertex_shader_create_info =
    ShaderCreateInfo::create()
        .vin(ShaderType::VEC2, 0, "aPos")
        .vin(ShaderType::VEC2, 1, "aTexCoord")
        .vinterface(outline_selected_vertex_vs_out, "vs_inout")
        .fout(ShaderType::VEC4, 0, "fragColor")
        .uniform(ShaderType::SAMPLER_2D, "colorTexture")
        .uniform(ShaderType::SAMPLER_2D, "entitiesTexture")
        .uniform(ShaderType::VEC3, "selectionColor")
        .uniform(ShaderType::FLOAT, "mixFactor")
        .uniform(ShaderType::INT, "selectedEntityID")
        .uniform(ShaderType::INT, "selectedVertexID")
        .vertex("outline_selected_vertex_vs.glsl")
        .fragment("outline_selected_vertex_fs.glsl");

#endif // OUTLINE_SELECTED_VERTEX_SHADER_INFO_H
