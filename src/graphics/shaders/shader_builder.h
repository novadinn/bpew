#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include <sstream>
#include <fstream>
#include <cstring>

#include <string>

#include "shader.h"
#include "shader_info.h"
#include "../../core/tokenizer.h"

struct ShaderCreateInfo;

struct ShaderBuilder {
    static Shader build_shader_from_create_info(const ShaderCreateInfo& create_info);
    static const char* from_type(ShaderType type);

private:    
    static void rec_include(std::stringstream& ss, const char* src); 
};
 

#endif
