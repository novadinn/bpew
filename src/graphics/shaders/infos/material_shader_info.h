#ifndef MATERIAL_SHADER_INFO_H
#define MATERIAL_SHADER_INFO_H

#include "../shader_create_info.h"

const ShaderInterfaceCreateInfo material_vs_out =
    ShaderInterfaceCreateInfo::create("VS_OUT")
        .smooth(ShaderType::VEC3, "fragPos")
        .smooth(ShaderType::VEC3, "normal")
        .smooth(ShaderType::VEC2, "texCoord");

const ShaderCreateInfo material_shader_create_info =
    ShaderCreateInfo::create()
        .vin(ShaderType::VEC3, 0, "aPos")
        .vin(ShaderType::VEC3, 1, "aNormal")
        .vin(ShaderType::VEC2, 2, "aTexCoord")
        .vin(ShaderType::VEC3, 3, "aTangent")
        .vin(ShaderType::VEC3, 4, "aBitangent")
        .vinterface(material_vs_out, "vs_inout")
        .fout(ShaderType::VEC4, 0, "fragColor")
        .fout(ShaderType::VEC4, 1, "entityID")
        .uniform(ShaderType::MAT4, "model")
        .uniform(ShaderType::MAT4, "view")
        .uniform(ShaderType::MAT4, "projection")
        .uniform("DirectionalLight", "dirLight")
        .uniform(ShaderType::FLOAT, "shininess")
        .uniform(ShaderType::INT, "currentEntityID")
        .uniform(ShaderType::VEC3, "viewPos")
        .dep("lights_lib.glsl")
        .vertex("material_shader_vs.glsl")
        .fragment("material_shader_fs.glsl");

#endif
