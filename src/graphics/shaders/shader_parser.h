#ifndef SHADER_PARSER_H
#define SHADER_PARSER_H

#include <fstream>

#include "shader_create_info.h"
#include "../../core/tokenizer.h"

// TODO: is this needed?
struct ShaderParser {
    ShaderCreateInfo parseCustomShader(const char* src);
};

#endif
