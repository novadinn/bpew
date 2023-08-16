#ifndef MATERIAL_SHADER_BUILDER_H
#define MATERIAL_SHADER_BUILDER_H

#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include "../../core/sha.h"
#include "../../nodes/node.h"
#include "shader_builder.h"

struct Material;
struct ShaderCreateInfo;
struct ShaderInterfaceCreateInfo;
struct ShaderContainer;

struct MaterialShaderBuilder : ShaderBuilder {
  static bool buildShaderFromShaderContainer(ShaderContainer *shader_container);
  static bool buildMaterialShader(Material &material);
  static bool buildMaterialRenderedShader(Material &material,
                                          uint num_spot_lights,
                                          uint num_point_lights,
                                          uint num_dir_lights);
  static const char *fromType(NodePropertyType type);
  static ShaderType toType(NodePropertyType type);
  static const char *getNodeName(NodeType type);
  static const char *getNodeSource(NodeType type);

  static void generateMaterialIds(Material &material);
  static void revertMaterialIds(Material &material);

private:
  static ShaderContainer *getShaderContainer(std::string &hash);
  static void buildNodeTree(std::stringstream &ss,
                            ShaderCreateInfo &create_info, Material &material);
  static void buildNode(std::stringstream &ss, Node *node, Material &material);
  static void buildNodeUniforms(ShaderCreateInfo &info, Node *node,
                                Material *material);
  static void buildNodeUniform(ShaderCreateInfo &info, Node *node,
                               const NodeInput &prop);
  static Sha generateMaterialSha(Material &material);
  static Sha generateMaterialRenderedSha(Material &material,
                                         uint num_spot_lights,
                                         uint num_point_lights,
                                         uint num_dir_lights);
  static void decreaseUsage(std::string &hash, ShaderContainer &container);
  static void increaseUsage(ShaderContainer &container);

  static std::map<std::string, ShaderContainer> shaders;
  static std::map<Material *, std::vector<int>> ids_buf;
};

#endif
