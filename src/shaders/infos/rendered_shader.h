#ifndef RENDERED_SHADER_H
#define RENDERED_SHADER_H

#include "../../graphics/shaders/shader_create_info.h"

#include "shader_interface_info.h"

const ShaderInterfaceCreateInfo rendered_vs_out = ShaderInterfaceCreateInfo::create("VS_OUT")
    .smooth(ShaderType::VEC3, "fragPos")
    .smooth(ShaderType::VEC3, "normal")
    .smooth(ShaderType::VEC2, "texCoord");

const ShaderCreateInfo rendered_shader_create_info = ShaderCreateInfo::create()
    .vin(ShaderType::VEC3, 0, "aPos")
    .vin(ShaderType::VEC3, 1, "aNormal")
    .vin(ShaderType::VEC2, 2, "aTexCoord")
    .vin(ShaderType::VEC3, 3, "aTangent")
    .vin(ShaderType::VEC3, 4, "aBitangent")
    .interface(rendered_vs_out, "vs_inout")
    .fout(ShaderType::VEC4, 0, "fragColor")
    .fout(ShaderType::VEC4, 1, "entityID")  
    .uniform(ShaderType::MAT4, "model")
    .uniform(ShaderType::MAT4, "view")
    .uniform(ShaderType::MAT4, "projection")
    .uniform("DirectionalLight", "dirLight")
    .uniform("PointLight", "pointLight")
    .uniform("SpotLight", "spotLight")
    .uniform(ShaderType::FLOAT, "shininess")
    .uniform(ShaderType::INT, "entity")
    .uniform(ShaderType::VEC3, "viewPos")
    .dep("lights_lib.glsl")
    .vertex("rendered_shader_vs.glsl")
    .fragment("rendered_shader_fs.glsl");	    

#endif // RENDERED_SHADER_H
