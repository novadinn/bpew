#ifndef SHADER_PARSER_H
#define SHADER_PARSER_H

#include <fstream>

#include "../../core/tokenizer.h"
#include "shader_create_info.h"

// TODO: is this needed?
struct ShaderParser {
  ShaderCreateInfo parseCustomShader(const char *src);
};

#endif
