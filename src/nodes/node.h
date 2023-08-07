#ifndef NODE_H
#define NODE_H

#include "../core/id.h"
#include "../graphics/texture.h"
#include "node_link.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

enum NodeType {
  // Input
  RGB,
  TEXTURE_COORDINATE,
  BEVEL,
  // Output
  MATERIAL_OUTPUT,
  // Shader
  PRINCIPLED_BSDF,
  // Texture
	BRICK_TEXTURE,
	CHECKER_TEXTURE,
	ENVIRONMENT_TEXTURE_EQUIRECTANGULAR,
	ENVIRONMENT_TEXTURE_MIRROR_BALL,
	ENVIRONMENT_TEXTURE_EMPTY,
  IMAGE_TEXTURE,
	MAGIC_TEXTURE,
	MUSGRAVE_TEXTURE,
	NOISE_TEXTURE,	
  // Color
  BRIGHTNESS_CONTRAST,
  GAMMA,
  INVERT,
	LIGHT_FALLOFF,
  // Vector
  // Converter
  MIX,
};

enum NodePropertyType {  
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
  uint texture_value;
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
  NodeLink *link = nullptr;
  NodePropertyValue value;
  NodePropertyType type;
  NodePropertySource source;
  bool enabled = true;

  bool useUniform(Material *material);
};

struct NodeOutput {
  void create(std::string prop_name, NodePropertyType prop_type) {
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
