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
    void build_shader_from_create_info(Shader& shader, const ShaderCreateInfo& create_info, const char* additional_info = "");
    void build_material_shader(Material& material);
    static const char* from_type(ShaderType type);
    static const char* from_type(NodePropertyType type);
    static const char* get_node_name(NodeType type);

    std::set<std::string> libs;
    Sha sha;
    
private:
    void include_libs(std::stringstream& ss);
    void include_lib(std::stringstream& ss, const char* dep);
    void proceed_source(const char* dep);
    void build_node(std::stringstream& ss, const Node* node, Material& material);
    void build_node_uniforms(ShaderCreateInfo& info, const Node* node);
    void build_node_uniform(ShaderCreateInfo& info, const Node* node, const NodeProperty& prop);
    Sha generate_material_sha(Material& material);
}; 

#endif
