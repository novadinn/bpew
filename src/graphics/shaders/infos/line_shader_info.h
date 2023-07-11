#ifndef LINE_SHADER_INFO_H
#define LINE_SHADER_INFO_H

#include "../shader_create_info.h"

const ShaderCreateInfo line_shader_create_info =
    ShaderCreateInfo::create()
        .vin(ShaderType::VEC3, 0, "aPos")
        .vin(ShaderType::VEC3, 1, "aColor")
        .vout(ShaderType::VEC3, "color")
        .uniform(ShaderType::MAT4, "view")
        .uniform(ShaderType::MAT4, "projection")
        .fout(ShaderType::VEC4, 0, "fragColor")
        .fin(ShaderType::VEC3, "color")
        .vertex("line_vs.glsl")
        .fragment("line_fs.glsl");

#endif
