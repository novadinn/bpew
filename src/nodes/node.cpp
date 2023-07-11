#include "node.h"

#include "../core/log.h"
#include "../graphics/shaders/shader.h"

#include <glm/glm.hpp>

void Node::create(NodeType node_type) {

  std::string node_name;
  std::vector<NodeProperty> node_inputs;
  std::vector<NodeProperty> node_outputs;

  switch (node_type) {
  case NodeType::RGB: {
    node_name = "RGB";

    NodeProperty prop;
    NodePropertyValue prop_value;

    // Color socket
    prop = {};
    prop_value = {};

    prop_value.vector3_value = glm::vec3(0.0f);

    prop.create(prop_value, "Color", NodePropertyType::VECTOR3);
    node_inputs.push_back(prop);

    // Color socket
    prop = {};
    prop_value = {};

    prop_value.color_value = glm::vec4(0.0f);

    prop.create(prop_value, "Color", NodePropertyType::COLOR);
    node_outputs.push_back(prop);

  } break;
  case NodeType::MATERIAL_OUTPUT: {
    node_name = "Material Output";

    NodeProperty prop;
    NodePropertyValue prop_value;

    // Surface socket
    prop = {};
    prop_value = {};

    prop_value.shader_value = Shader{};

    prop.create(prop_value, "Surface", NodePropertyType::SHADER);
    node_inputs.push_back(prop);

    // Volume socket
    prop = {};
    prop_value = {};

    prop_value.shader_value = Shader{};

    prop.create(prop_value, "Volume", NodePropertyType::SHADER);
    node_inputs.push_back(prop);

    // Displacement socket
    prop = {};
    prop_value = {};

    prop_value.vector3_value = glm::vec3(0.0f);
    ;

    prop.create(prop_value, "Displacement", NodePropertyType::VECTOR3);
    node_inputs.push_back(prop);
  } break;
  default: {
    LOG_ERROR("Undefined node type\n");
  } break;
  }

  id.create(node_name);
  inputs = node_inputs;
  outputs = node_outputs;
  type = node_type;
}
