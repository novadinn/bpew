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
  // Texture
  IMAGE_TEXTURE,
  // Color
  // Vector
  // Converter
};


enum NodePropertyType {
  COLOR,
  VECTOR3,
  VECTOR2,
  FLOAT,
  INT,
  ENUM,
  SHADER,
  TEXTURE,
};

enum NodePropertySource {
    UNIFORM,
    ATTR,
    VS_OUT,
};

union NodePropertyValue {
  glm::vec4 color_value;
  glm::vec3 vector3_value;
  glm::vec2 vector2_value;
  float float_value;
  int int_value;
  int enum_value;
  Shader shader_value;
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
};

struct Node {
  void create(NodeType node_type);    
    
  ID id;
  std::vector<NodeInput> inputs;
  std::vector<NodeOutput> outputs;    
    
  NodeType type;
};

#endif // NODE_H
