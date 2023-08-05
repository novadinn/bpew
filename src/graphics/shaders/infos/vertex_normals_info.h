#ifndef VERTEX_NORMALS_INFO_H
#define VERTEX_NORMALS_INFO_H

#include "../shader_create_info.h"

const ShaderInterfaceCreateInfo vertex_normals_vs_out =
    ShaderInterfaceCreateInfo::create("VS_OUT").smooth(ShaderType::VEC3,
                                                       "normal");

const ShaderCreateInfo vertex_normals_shader_create_info =
    ShaderCreateInfo::create()
        .vin(ShaderType::VEC3, 0, "aPos")
        .vin(ShaderType::VEC3, 1, "aNormal")
        .vin(ShaderType::VEC2, 2, "aTexCoord")
        .vin(ShaderType::VEC3, 3, "aTangent")
        .vin(ShaderType::VEC3, 4, "aBitangent")
        .vinterface(vertex_normals_vs_out, "vs_inout")
        .fout(ShaderType::VEC4, 0, "fragColor")
        .uniform(ShaderType::MAT4, "model")
        .uniform(ShaderType::MAT4, "view")
        .uniform(ShaderType::MAT4, "projection")
        .uniform(ShaderType::VEC4, "color")
        .uniform(ShaderType::FLOAT, "normalLength")
        .vertex("vertex_normals_vs.glsl")
        .fragment("vertex_normals_fs.glsl")
        .primitive(GeometryInType::TRIANGLES, GeometryOutType::LINE_STRIP, 6)
        .geometry("vertex_normals_gs.glsl");

#endif // VERTEX_NORMALS_INFO_H