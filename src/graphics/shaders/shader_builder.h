#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include "../../core/sha.h"
#include "../../core/tokenizer.h"
#include "../../core/utils.h"
#include "../../nodes/node.h"
#include "shader.h"
#include "shader_info.h"

struct Material;
struct ShaderCreateInfo;
struct ShaderInterfaceCreateInfo;
struct ShaderContainer;

struct ShaderBuilder { 
  static bool buildShaderFromCreateInfo(Shader &shader, const ShaderCreateInfo &info);
  static bool buildShaderFromShaderContainer(ShaderContainer *shader_container);
  static bool buildMaterialShader(Material &material);
  static bool buildMaterialRenderedShader(Material &material,
                                          uint num_spot_lights,
                                          uint num_point_lights,
                                          uint num_dir_lights);
  static const char *fromType(ShaderType type);
  static const char *fromType(NodePropertyType type);
  static const char *fromType(InterpolationType type);
	static const char *fromType(GeometryInType type);
	static const char *fromType(GeometryOutType type);
  static ShaderType toType(NodePropertyType type);
  static const char *getNodeName(NodeType type);

	static void generateMaterialIds(Material &material);
	static void revertMaterialIds(Material &material);

private:
	static void buildDefines(std::stringstream &ss, ShaderCreateInfo &create_info);
	static void buildVertexShaderDefines(std::stringstream &vs, ShaderCreateInfo &create_info);
	static void buildFragmentShaderDefines(std::stringstream &fs, ShaderCreateInfo &create_info);
	static void buildGeometryShaderDefines(std::stringstream &gs, ShaderCreateInfo &create_info);
	
  static ShaderContainer *getShaderContainer(std::string &hash);
  static void buildNodeTree(std::stringstream &ss,
                            ShaderCreateInfo &create_info, Material &material);
  static void includeLibs(std::stringstream &ss, ShaderCreateInfo &create_info);
  static bool includeLib(std::stringstream &ss, const char *dep);
  static void proceedSource(const char *dep, ShaderCreateInfo &create_info);
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
  static void buildInterface(std::stringstream &ss,
                             ShaderInterfaceInfo &interface_info);
  static void decreaseUsage(std::string &hash, ShaderContainer &container);
  static void increaseUsage(ShaderContainer &container); 
	
  static std::map<std::string, ShaderContainer> shaders;
	static std::map<Material*, std::vector<int>> ids_buf;
};

#endif
