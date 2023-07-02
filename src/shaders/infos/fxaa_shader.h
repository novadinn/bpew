#ifndef FXAA_SHADER_H
#define FXAA_SHADER_H

#include "../../graphics/shaders/shader_create_info.h"

const ShaderInterfaceCreateInfo fxaa_vs_out = ShaderInterfaceCreateInfo::create("VS_OUT")
    .noperspective(ShaderType::VEC2, "texCoordNoPInterp");

const ShaderCreateInfo fxaa_shader_create_info = ShaderCreateInfo::create()
    .vin(ShaderType::VEC2, 0, "aPos")
    .vin(ShaderType::VEC2, 1, "aTexCoord")
    .interface(fxaa_vs_out, "vs_inout")
    .fout(ShaderType::VEC4, 0, "fragColor")
    .uniform(ShaderType::SAMPLER_2D, "colorTexture")
    .uniform(ShaderType::VEC2, "viewportSize")
    .dep("fxaa_lib.glsl")
    .vertex("fxaa_vs.glsl")
    .fragment("fxaa_fs.glsl");

#endif // FXAA_SHADER_H
