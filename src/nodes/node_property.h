#ifndef NODE_PROPERTY_H
#define NODE_PROPERTY_H

#include "../graphics/shader.h"
#include "../graphics/texture.h"

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

struct NodeProperty {
  union {
    glm::vec4 color_value;
    glm::vec3 vector3_value;
    glm::vec3 vector2_value;
    float float_value;
    int int_value;
    int enum_value;
    Shader shader_value;
    Texture2D texture_value;
  };
  std::string name;
  std::vector<NodeProperty*> links;
  NodePropertyType type;
};

#endif // NODE_PROPERTY_H
