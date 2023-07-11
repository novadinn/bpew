#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include "../../core/sha.h"
#include "../../core/tokenizer.h"
#include "../../nodes/node.h"
#include "../../nodes/node_property.h"
#include "shader_info.h"

struct Material;
struct ShaderCreateInfo;
struct ShaderInterfaceCreateInfo;
struct ShaderContainer;

struct ShaderBuilder {
  static bool buildShaderFromCreateInfo(Shader &shader,
                                        const ShaderCreateInfo &info,
                                        const char *additional_info = "");
  static void buildShaderFromShaderContainer(ShaderContainer *shader_container);
  static void buildMaterialShader(Material &material);
  static void buildMaterialRenderedShader(Material &material,
                                          uint num_spot_lights,
                                          uint num_point_lights,
                                          uint num_dir_lights);
  static const char *fromType(ShaderType type);
  static const char *fromType(NodePropertyType type);
  static const char *fromType(InterpolationType type);
  static ShaderType toType(NodePropertyType type);
  static const char *getNodeName(NodeType type);

private:
  static ShaderContainer *getShaderContainer(std::string &hash);
  static void buildNodeTree(std::stringstream &ss,
                            ShaderCreateInfo &create_info, Material &material);
  static void includeLibs(std::stringstream &ss, ShaderCreateInfo &create_info);
  static void includeLib(std::stringstream &ss, const char *dep);
  static void proceedSource(const char *dep, ShaderCreateInfo &create_info);
  static void buildNode(std::stringstream &ss, const Node *node,
                        Material &material);
  static void buildNodeUniforms(ShaderCreateInfo &info, const Node *node);
  static void buildNodeUniform(ShaderCreateInfo &info, const Node *node,
                               const NodeProperty &prop);
  // TODO: should be placed at here?
  static Sha generateMaterialSha(Material &material);
  static Sha generateMaterialRenderedSha(Material &material,
                                         uint num_spot_lights,
                                         uint num_point_lights,
                                         uint num_dir_lights);
  static void buildInterface(std::stringstream &ss,
                             ShaderInterfaceInfo &interface_info);
  static void decreaseUsage(std::string &hash, ShaderContainer &container);
  static void increaseUsage(ShaderContainer &container);

  static std::map<std::string, ShaderContainer> shaders;
};

#endif
