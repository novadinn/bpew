#ifndef LINE_SHADER_INFO_H
#define LINE_SHADER_INFO_H

#include "../shader_create_info.h"

const ShaderInterfaceCreateInfo overlay_grid_vs_out =
    ShaderInterfaceCreateInfo::create("VS_OUT").smooth(ShaderType::VEC3,
                                                       "vertexPosition");

const ShaderCreateInfo overlay_grid_shader_create_info =
    ShaderCreateInfo::create()
        .vin(ShaderType::VEC3, 0, "aPos")
        .vin(ShaderType::VEC3, 1, "aColor")
        .vout(ShaderType::VEC3, "color")
        .vinterface(overlay_grid_vs_out, "vs_inout")
        .uniform(ShaderType::MAT4, "view")
        .uniform(ShaderType::MAT4, "projection")
        .uniform(ShaderType::VEC3, "cameraPosition")
        .uniform(ShaderType::FLOAT, "far")
        .fout(ShaderType::VEC4, 0, "fragColor")
        .fin(ShaderType::VEC3, "color")
        .vertex("overlay_grid_vs.glsl")
        .fragment("overlay_grid_fs.glsl");

#endif
