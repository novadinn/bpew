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
    void buildShaderFromCreateInfo(Shader& shader, const ShaderCreateInfo& create_info, const char* additional_info = "");
    void buildMaterialShader(Material& material);
    static const char* fromType(ShaderType type);
    static const char* fromType(NodePropertyType type);
    static const char* getNodeName(NodeType type);

    std::set<std::string> libs;
    Sha sha;
    
private:
    void includeLibs(std::stringstream& ss);
    void includeLib(std::stringstream& ss, const char* dep);
    void proceedSource(const char* dep);
    void buildNode(std::stringstream& ss, const Node* node, Material& material);
    void buildNodeUniforms(ShaderCreateInfo& info, const Node* node);
    void buildNodeUniform(ShaderCreateInfo& info, const Node* node, const NodeProperty& prop);
    Sha generateMaterialSha(Material& material);
}; 

#endif
