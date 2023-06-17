#ifndef NODE_PROPERTY_H
#define NODE_PROPERTY_H

#include "../graphics/shaders/shader.h"
#include "../graphics/texture.h"
#include "../core/id.h"
#include "node_link.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

enum NodePropertyType {
  COLOR,
  VECTOR3,
  VECTOR2,
  FLOAT,
  INT,
  ENUM,
  SHADER,
  TEXTURE
};

union NodePropertyValue {
  glm::vec4 color_value;
  glm::vec3 vector3_value;
  glm::vec3 vector2_value;
  float float_value;
  int int_value;
  int enum_value;
  Shader shader_value;
  Texture2D texture_value;
};

struct NodeProperty {
  void create(NodePropertyValue prop_value, std::string prop_name,
	      NodePropertyType prop_type) {
    id.create(prop_name);
    value = prop_value;
    type = prop_type;
  }

  ID id;        
  std::vector<NodeLink> links;
  NodePropertyValue value;
  NodePropertyType type;
};

#endif // NODE_PROPERTY_H
