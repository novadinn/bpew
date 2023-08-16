#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#include "../../core/tokenizer.h"
#include "../../core/utils.h"
#include "shader.h"
#include "shader_info.h"

struct Material;
struct ShaderCreateInfo;
struct ShaderInterfaceCreateInfo;
struct ShaderContainer;

struct ShaderBuilder {
  static bool buildShaderFromCreateInfo(Shader &shader,
                                        const ShaderCreateInfo &info);
  static const char *fromType(ShaderType type);
  static const char *fromType(InterpolationType type);
  static const char *fromType(GeometryInType type);
  static const char *fromType(GeometryOutType type);

protected:
  static void buildDefines(std::stringstream &ss,
                           ShaderCreateInfo &create_info);
  static void buildVertexShaderDefines(std::stringstream &vs,
                                       ShaderCreateInfo &create_info);
  static void buildFragmentShaderDefines(std::stringstream &fs,
                                         ShaderCreateInfo &create_info);
  static void buildGeometryShaderDefines(std::stringstream &gs,
                                         ShaderCreateInfo &create_info);

  static void includeLibs(std::stringstream &ss, std::set<std::string> &libs);
  static bool includeLib(std::stringstream &ss, const char *dep,
                         std::set<std::string> &included_libs);
  static void buildInterface(std::stringstream &ss,
                             ShaderInterfaceInfo &interface_info);
  static void decreaseUsage(std::string &hash, ShaderContainer &container);
  static void increaseUsage(ShaderContainer &container);
};

#endif
