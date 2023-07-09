#include "node.h"

#include "../core/log.h"
#include "../graphics/shaders/shader.h"

#include <glm/glm.hpp>

void Node::create(NodeType node_type) {

  std::string node_name;
  std::vector<NodeInput> node_inputs;
  std::vector<NodeOutput> node_outputs;
  
  switch(node_type) {
  case NodeType::RGB: {    
    node_name = "RGB";

    NodeInput input;
    NodeOutput output;
    NodePropertyValue prop_value;

    // Color socket
    input = {};
    prop_value = {};
    
    prop_value.vector3_value = glm::vec3(0.0f);
    
    input.create(prop_value, "Color", NodePropertyType::VECTOR3, NodePropertySource::UNIFORM);
    node_inputs.push_back(input);

    // Color socket
    output = {};
    
    output.create("Color", NodePropertyType::COLOR);
    node_outputs.push_back(output);
    
  } break;
  case NodeType::MATERIAL_OUTPUT: {
    node_name = "Material Output";

    NodeInput prop;    
    NodePropertyValue prop_value;

    // Surface socket
    prop = {};
    prop_value = {};
    
    prop_value.shader_value = Shader{};
    
    prop.create(prop_value, "Surface", NodePropertyType::SHADER, NodePropertySource::ATTR);
    node_inputs.push_back(prop);

    // Volume socket
    prop = {};
    prop_value = {};

    prop_value.shader_value = Shader{};
    
    prop.create(prop_value, "Volume", NodePropertyType::SHADER, NodePropertySource::ATTR);
    node_inputs.push_back(prop);

    // Displacement socket
    prop = {};
    prop_value = {};

    prop_value.vector3_value = glm::vec3(0.0f);;
    
    prop.create(prop_value, "Displacement", NodePropertyType::VECTOR3, NodePropertySource::ATTR);
    node_inputs.push_back(prop);
  } break;
  case NodeType::IMAGE_TEXTURE: {
      node_name = "Image Texture";

      NodeInput input;
      NodeOutput output;
      NodePropertyValue input_value;

      input = {};
      input_value = {};
      
      input_value.vector2_value = glm::vec2(0.0f);

      input.create(input_value, "Texture", NodePropertyType::TEXTURE, NodePropertySource::UNIFORM);
      node_inputs.push_back(input);
      
      input.create(input_value, "Vector", NodePropertyType::VECTOR2, NodePropertySource::ATTR);
      node_inputs.push_back(input);

      input = {};
      input_value = {};

      input_value.texture_value = Texture2D{};           

      output = {};      

      output.create("Color", NodePropertyType::COLOR);
      node_outputs.push_back(output);

      output = {};
      
      output.create("Alpha", NodePropertyType::FLOAT);
      node_outputs.push_back(output);
  } break;
  case NodeType::TEXTURE_COORDINATE: {
      node_name = "Texture Coordinate";

      NodeOutput output;
      NodeInput input;
      NodePropertyValue input_value;

      input = {};
      input_value = {};

      input_value.vector2_value = glm::vec2(0.0f);

      input.create(input_value, "texCoord", NodePropertyType::VECTOR2, NodePropertySource::VS_OUT);      
      node_inputs.push_back(input);
      
      output = {};

      output.create("UV", NodePropertyType::VECTOR2);
      node_outputs.push_back(output);
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
