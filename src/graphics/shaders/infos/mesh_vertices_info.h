#ifndef MESH_VERTICES_INFO_H
#define MESH_VERTICES_INFO_H

#include "../shader_create_info.h"

const ShaderInterfaceCreateInfo mesh_vertices_vs_out = ShaderInterfaceCreateInfo::create("VS_OUT")
    .flat(ShaderType::INT, "vertexID");

const ShaderCreateInfo mesh_vertices_shader_create_info = ShaderCreateInfo::create()
    .vin(ShaderType::VEC3, 0, "aPos")
    .vin(ShaderType::VEC3, 1, "aNormal")
    .vin(ShaderType::VEC2, 2, "aTexCoord")
    .vin(ShaderType::VEC3, 3, "aTangent")
    .vin(ShaderType::VEC3, 4, "aBitangent")
    .interface(mesh_vertices_vs_out, "vs_inout")
    .fout(ShaderType::VEC4, 0, "fragColor")
    .fout(ShaderType::VEC4, 1, "vertexID")  
    .uniform(ShaderType::MAT4, "model")
    .uniform(ShaderType::MAT4, "view")
    .uniform(ShaderType::MAT4, "projection")
    .uniform(ShaderType::VEC3, "color")
    .uniform(ShaderType::VEC3, "selectionColor")
    .uniform(ShaderType::FLOAT, "mixFactor")
    .uniform(ShaderType::INT, "currentEntityID")
    .uniform(ShaderType::INT, "selectedEntityID")
    .uniform(ShaderType::INT, "selectedVertexID")
    .vertex("mesh_vertices_vs.glsl")
    .fragment("mesh_vertices_fs.glsl");


#endif // MESH_VERTICES_INFO_H
