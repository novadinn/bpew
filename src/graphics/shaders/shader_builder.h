#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#include <sstream>
#include <fstream>
#include <cstring>
#include <set>
#include <string>

#include "shader.h"
#include "shader_info.h"
#include "../../core/tokenizer.h"
#include "../../nodes/node.h"
#include "../../nodes/node_property.h"
#include "../../core/sha.h"

struct Material;
struct ShaderCreateInfo;

struct ShaderBuilder {
    static void buildShaderFromCreateInfo(Shader& shader, const ShaderCreateInfo& info, const char* additional_info = "");
    static void buildMaterialShader(Material& material);
    static const char* fromType(ShaderType type);
    static const char* fromType(NodePropertyType type);
    static const char* getNodeName(NodeType type);       
    
private:
    static void includeLibs(std::stringstream& ss, ShaderCreateInfo& create_info);
    static void includeLib(std::stringstream& ss, const char* dep);
    static void proceedSource(const char* dep, ShaderCreateInfo& create_info);
    static void buildNode(std::stringstream& ss, const Node* node, Material& material);
    static void buildNodeUniforms(ShaderCreateInfo& info, const Node* node);
    static void buildNodeUniform(ShaderCreateInfo& info, const Node* node, const NodeProperty& prop);
    // TODO: should be placed at here?
    static Sha generateMaterialSha(Material& material);
}; 

#endif
