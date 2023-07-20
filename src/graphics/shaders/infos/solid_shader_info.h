#ifndef SOLID_SHADER_INFO_H
#define SOLID_SHADER_INFO_H

#include "../shader_create_info.h"

const ShaderInterfaceCreateInfo solid_vs_out =
	ShaderInterfaceCreateInfo::create("VS_OUT")
	.smooth(ShaderType::VEC3, "fragPos")
	.smooth(ShaderType::VEC3, "normal");

const ShaderInterfaceCreateInfo solid_gs_out =
	ShaderInterfaceCreateInfo::create("GS_OUT")
	.smooth(ShaderType::VEC3, "fragPos")
	.smooth(ShaderType::VEC3, "normal");

const ShaderCreateInfo solid_shader_create_info =
	ShaderCreateInfo::create()
	.vin(ShaderType::VEC3, 0, "aPos")
	.vin(ShaderType::VEC3, 1, "aNormal")
	.vin(ShaderType::VEC2, 2, "aTexCoord")
	.vin(ShaderType::VEC3, 3, "aTangent")
	.vin(ShaderType::VEC3, 4, "aBitangent")
	.vinterface(solid_vs_out, "vs_inout")	
	.fout(ShaderType::VEC4, 0, "fragColor")
	.fout(ShaderType::VEC4, 1, "entityID")
	.uniform(ShaderType::MAT4, "model")
	.uniform(ShaderType::MAT4, "view")
	.uniform(ShaderType::MAT4, "projection")
	.uniform("DirectionalLight", "dirLight")
	.uniform(ShaderType::FLOAT, "shininess")
	.uniform(ShaderType::VEC3, "viewPos")
	.uniform(ShaderType::INT, "currentEntityID")
	.dep("lights_lib.glsl")
	.vertex("solid_shader_vs.glsl")
	.fragment("solid_shader_fs.glsl");

const ShaderCreateInfo geometry_example_create_info =
	ShaderCreateInfo::create()
	.vin(ShaderType::VEC3, 0, "aPos")
	.vin(ShaderType::VEC3, 1, "aNormal")
	.vin(ShaderType::VEC2, 2, "aTexCoord")
	.vin(ShaderType::VEC3, 3, "aTangent")
	.vin(ShaderType::VEC3, 4, "aBitangent")
	.vinterface(solid_vs_out, "vs_inout")
	.ginterface(solid_gs_out, "gs_out")
	.fout(ShaderType::VEC4, 0, "fragColor")
	.fout(ShaderType::VEC4, 1, "entityID")
	.uniform(ShaderType::MAT4, "model")
	.uniform(ShaderType::MAT4, "view")
	.uniform(ShaderType::MAT4, "projection")
	.uniform("DirectionalLight", "dirLight")
	.uniform(ShaderType::FLOAT, "shininess")
	.uniform(ShaderType::VEC3, "viewPos")
	.uniform(ShaderType::INT, "currentEntityID")
	.dep("lights_lib.glsl")
	.vertex("solid_shader_vs.glsl")
	.fragment("solid_shader_fs.glsl")
	.primitive(GeometryInType::TRIANGLES, GeometryOutType::TRIANGLE_STRIP, 6)
	.geometry("solid_shader_gs.glsl");


#endif
