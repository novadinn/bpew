#ifndef NODE_H
#define NODE_H

#include "../core/id.h"
#include "node_link.h"
#include "../graphics/shaders/shader.h"
#include "../graphics/texture.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

enum NodeType {
  // Input
  RGB,
  TEXTURE_COORDINATE,
  // Output
  MATERIAL_OUTPUT,
  // Shader
  PRINCIPLED_BSDF,
  // Texture
  IMAGE_TEXTURE,
  // Color
  // Vector
  // Converter
  MIX,
};


enum NodePropertyType {
  COLOR,
  VECTOR4,
  VECTOR3,
  VECTOR2,
  FLOAT,
  INT,
  ENUM,
  TEXTURE,  
};

enum NodePropertySource {
    UNIFORM,
    INPUT_UNIFORM,
    OUTPUT,    
    VS_OUT,
    OUTPUT_UNIFORM,    
};

union NodePropertyValue {
  glm::vec4 vector4_value;
  glm::vec3 vector3_value;
  glm::vec2 vector2_value;
  float float_value;
  int int_value;
  int enum_value;
  Texture2D texture_value;
};

struct NodeInput {
    void create(NodePropertyValue prop_value, std::string prop_name,
		NodePropertyType prop_type, NodePropertySource prop_source) {
	id.create(prop_name);
	value = prop_value;
	type = prop_type;
	source = prop_source;
    }

    ID id;
    NodeLink* link = nullptr;
    NodePropertyValue value;
    NodePropertyType type;
    NodePropertySource source;
    bool enabled = true;

    bool useUniform(Material *material);
};

struct NodeOutput {
    void create(std::string prop_name,
		NodePropertyType prop_type) {
	id.create(prop_name);	
	type = prop_type;
    }
    
    ID id;
    std::vector<NodeLink> links;
    NodePropertyType type;
    bool enabled = true;   
};

struct Node {
  void create(NodeType node_type);    
    
  ID id;
  std::vector<NodeInput> inputs;
  std::vector<NodeOutput> outputs;    
    
  NodeType type;
};

#endif // NODE_H
