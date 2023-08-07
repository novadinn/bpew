#include "node.h"

#include "../core/log.h"
#include "../graphics/shaders/shader.h"

#include <glm/glm.hpp>

void Node::create(NodeType node_type) {

  std::string node_name;
  std::vector<NodeInput> node_inputs;
  std::vector<NodeOutput> node_outputs;

  switch (node_type) {
  case NodeType::RGB: {
    node_name = "RGB";

    NodeInput input;
    NodeOutput output;
    NodePropertyValue prop_value;

    // Color socket
    input = {};
    prop_value = {};

    prop_value.vector4_value = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    input.create(prop_value, "Color", NodePropertyType::VECTOR4,
                 NodePropertySource::INPUT_UNIFORM);
    node_inputs.push_back(input);

    // Color socket
    output = {};

    output.create("Color", NodePropertyType::VECTOR4);
    node_outputs.push_back(output);
  } break;
  case NodeType::BEVEL: {
    node_name = "Bevel";

    NodeInput input;
    NodeOutput output;
    NodePropertyValue input_value;

    input = {};
    input_value = {};

    input_value.float_value = 0.05f;

    input.create(input_value, "Radius", NodePropertyType::FLOAT,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.vector3_value = glm::vec3(0.0f);

    input.create(input_value, "Normal", NodePropertyType::VECTOR3,
                 NodePropertySource::OUTPUT);
    node_inputs.push_back(input);

    // TODO: samples not used ?

    output = {};

    output.create("Normal", NodePropertyType::VECTOR3);
    node_outputs.push_back(output);
  } break;
  case NodeType::MATERIAL_OUTPUT: {
    node_name = "Material Output";

    NodeInput prop;
    NodePropertyValue prop_value;

    // Surface socket
    prop = {};
    prop_value = {};

    prop_value.vector4_value = glm::vec4(0.0f);

    prop.create(prop_value, "Surface", NodePropertyType::VECTOR4,
                NodePropertySource::OUTPUT);
    node_inputs.push_back(prop);

    // Volume socket
    prop = {};
    prop_value = {};

    prop_value.vector4_value = glm::vec4(0.0f);

    prop.create(prop_value, "Volume", NodePropertyType::VECTOR4,
                NodePropertySource::OUTPUT);
    node_inputs.push_back(prop);

    // Displacement socket
    prop = {};
    prop_value = {};

    prop_value.vector4_value = glm::vec4(0.0f);

    prop.create(prop_value, "Displacement", NodePropertyType::VECTOR4,
                NodePropertySource::OUTPUT);
    node_inputs.push_back(prop);
  } break;
	case NodeType::BRICK_TEXTURE: {
		node_name = "Brick Texture";

		NodeInput input;
		NodeOutput output;
		NodePropertyValue input_value;

		input = {};
		input_value = {};

		input_value.vector3_value = glm::vec3(0.0f);

		input.create(input_value, "Vector", NodePropertyType::VECTOR3,
								 NodePropertySource::OUTPUT);		
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.vector4_value = glm::vec4(1.0f);

		input.create(input_value, "Color1", NodePropertyType::VECTOR4,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.vector4_value = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

		input.create(input_value, "Color2", NodePropertyType::VECTOR4,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.vector4_value = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		input.create(input_value, "Mortar", NodePropertyType::VECTOR4,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 5.0f;

		input.create(input_value, "Scale", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.02f;

		input.create(input_value, "Mortar size", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.1f;

		input.create(input_value, "Mortar Smooth", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.0f;

		input.create(input_value, "Bias", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.5f;

		input.create(input_value, "Brick Width", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.25f;

		input.create(input_value, "Row Height", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.5f;

		input.create(input_value, "Offset", NodePropertyType::FLOAT,
								 NodePropertySource::INPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.int_value = 2;

		input.create(input_value, "Frequency", NodePropertyType::INT,
								 NodePropertySource::INPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 1.0f;

		input.create(input_value, "Squash", NodePropertyType::FLOAT,
								 NodePropertySource::INPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.int_value = 2;

		input.create(input_value, "Frequency", NodePropertyType::INT,
								 NodePropertySource::INPUT_UNIFORM);
		node_inputs.push_back(input);

		output = {};

		output.create("Color", NodePropertyType::VECTOR4);
		node_outputs.push_back(output);

		output = {};

		output.create("Fac", NodePropertyType::FLOAT);
		node_outputs.push_back(output);
	} break;
	case NodeType::CHECKER_TEXTURE: {
		node_name = "Checker Texture";

		NodeInput input;
		NodeOutput output;
		NodePropertyValue input_value;

		input = {};
		input_value = {};

		input_value.vector3_value = glm::vec3(0.0f);

		input.create(input_value, "Vector", NodePropertyType::VECTOR3,
								 NodePropertySource::OUTPUT);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.vector4_value = glm::vec4(1.0f);

		input.create(input_value, "Color1", NodePropertyType::VECTOR4,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.vector4_value = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

		input.create(input_value, "Color2", NodePropertyType::VECTOR4,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 5.0f;

		input.create(input_value, "Scale", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		output = {};

		output.create("Color", NodePropertyType::VECTOR4);
		node_outputs.push_back(output);

		output = {};

		output.create("Fac", NodePropertyType::FLOAT);
		node_outputs.push_back(output);
	} break;
	case NodeType::ENVIRONMENT_TEXTURE_EQUIRECTANGULAR:
	case NodeType::ENVIRONMENT_TEXTURE_MIRROR_BALL:
	case NodeType::ENVIRONMENT_TEXTURE_EMPTY: {
		node_name = "Environment Texture";

		NodeInput input;
		NodeOutput output;
		NodePropertyValue input_value;

		input = {};
		input_value = {};

		input_value.vector3_value = glm::vec3(0.0f);

		input.create(input_value, "Vector", NodePropertyType::VECTOR3,
								 NodePropertySource::OUTPUT);
		node_inputs.push_back(input);

		output = {};

		output.create("Color", NodePropertyType::VECTOR4);
		node_outputs.push_back(output);
	} break;
  case NodeType::IMAGE_TEXTURE: {
    node_name = "Image Texture";

    NodeInput input;
    NodeOutput output;
    NodePropertyValue input_value;

    input = {};
    input_value = {};

    input_value.texture_value = 0;

    input.create(input_value, "Texture", NodePropertyType::TEXTURE,
                 NodePropertySource::INPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.vector3_value = glm::vec3(0.0f);

    input.create(input_value, "Vector", NodePropertyType::VECTOR3,
                 NodePropertySource::OUTPUT);
    node_inputs.push_back(input);

    output = {};

    output.create("Color", NodePropertyType::VECTOR4);
    node_outputs.push_back(output);

    output = {};

    output.create("Alpha", NodePropertyType::FLOAT);
    node_outputs.push_back(output);
  } break;
	case NodeType::MAGIC_TEXTURE: {
		node_name = "Magic Texture";

		NodeInput input;
		NodeOutput output;
		NodePropertyValue input_value;

		input = {};
		input_value = {};

		input_value.vector3_value = glm::vec3(0.0f);

		input.create(input_value, "Vector", NodePropertyType::VECTOR3,
								 NodePropertySource::OUTPUT);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 5.0f;

		input.create(input_value, "Scale", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);
		
		input = {};
		input_value = {};

		input_value.float_value = 1.0f;

		input.create(input_value, "Distortion", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.int_value = 2;

		input.create(input_value, "Depth", NodePropertyType::INT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		output = {};

		output.create("Color", NodePropertyType::VECTOR4);
		node_outputs.push_back(output);

		output = {};

		output.create("Fac", NodePropertyType::FLOAT);
		node_outputs.push_back(output);
	} break;
	case NodeType::MUSGRAVE_TEXTURE: {
		node_name = "Musgrave Texture";

		NodeInput input;
		NodeOutput output;
		NodePropertyValue input_value;

		input = {};
		input_value = {};

		input_value.enum_value = 2;

		input.create(input_value, "Dimensions", NodePropertyType::ENUM,
								 NodePropertySource::INPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.enum_value = 3;

		input.create(input_value, "Type", NodePropertyType::ENUM,
								 NodePropertySource::INPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.vector3_value = glm::vec3(0.0f);

		input.create(input_value, "Vector", NodePropertyType::VECTOR3,
								 NodePropertySource::OUTPUT);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.0f;

		input.create(input_value, "W", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 5.0f;

		input.create(input_value, "Scale", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 2.0f;

		input.create(input_value, "Detail", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 2.0f;

		input.create(input_value, "Dimension", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 2.0f;

		input.create(input_value, "Lacunarity", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.0f;

		input.create(input_value, "Offset", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 1.0f;

		input.create(input_value, "Gain", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		output = {};

		output.create("Height", NodePropertyType::FLOAT);
		node_outputs.push_back(output);
	} break;
	case NodeType::NOISE_TEXTURE: {
		node_name = "Noise Texture";

		NodeInput input;
		NodeOutput output;
		NodePropertyValue input_value;

		input = {};
		input_value = {};

		input_value.enum_value = 2;

		input.create(input_value, "Dimensions", NodePropertyType::ENUM,
								 NodePropertySource::INPUT_UNIFORM);
		node_inputs.push_back(input);
		
		input = {};
		input_value = {};

		input_value.vector3_value = glm::vec3(0.0f);

		input.create(input_value, "Vector", NodePropertyType::VECTOR3,
								 NodePropertySource::OUTPUT);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.0f;

		input.create(input_value, "W", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 5.0f;

		input.create(input_value, "Scale", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 2.0f;

		input.create(input_value, "Detail", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.5f;

		input.create(input_value, "Roughness", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.0f;

		input.create(input_value, "Distortion", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		output = {};

		output.create("Fac", NodePropertyType::FLOAT);
		node_outputs.push_back(output);

		output = {};

		output.create("Color", NodePropertyType::VECTOR4);
		node_outputs.push_back(output);
	} break;
  case NodeType::BRIGHTNESS_CONTRAST: {
    node_name = "Brightness/Contrast";

    NodeInput input;
    NodeOutput output;
    NodePropertyValue input_value;

    input = {};
    input_value = {};

    input_value.vector4_value = glm::vec4(1.0f);

    input.create(input_value, "Color", NodePropertyType::VECTOR4,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.float_value = 0.0f;

    input.create(input_value, "Brightness", NodePropertyType::FLOAT,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.float_value = 0.0f;

    input.create(input_value, "Contrast", NodePropertyType::FLOAT,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    output = {};

    output.create("Color", NodePropertyType::VECTOR4);
    node_outputs.push_back(output);
  } break;
  case NodeType::GAMMA: {
    node_name = "Gamma";

    NodeOutput output;
    NodeInput input;
    NodePropertyValue input_value;

    input = {};
    input_value = {};

    input_value.vector4_value = glm::vec4(1.0f);

    input.create(input_value, "Color", NodePropertyType::VECTOR4,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.float_value = 1.0f;

    input.create(input_value, "Gamma", NodePropertyType::FLOAT,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    output = {};

    output.create("Color", NodePropertyType::VECTOR4);
    node_outputs.push_back(output);
  } break;
  case NodeType::INVERT: {
    node_name = "Invert";

    NodeInput input;
    NodeOutput output;
    NodePropertyValue input_value;

    input = {};
    input_value = {};

    input_value.float_value = 1.0f;

    input.create(input_value, "Fac", NodePropertyType::FLOAT,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.vector4_value = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    input.create(input_value, "Color", NodePropertyType::VECTOR4,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    output = {};

    output.create("Color", NodePropertyType::VECTOR4);
    node_outputs.push_back(output);
  } break;
	case NodeType::LIGHT_FALLOFF: {
		node_name = "Light Falloff";

		NodeInput input;
		NodeOutput output;
		NodePropertyValue input_value;

		input = {};
		input_value = {};

		input_value.float_value = 100.0f;

		input.create(input_value, "Strength", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		input = {};
		input_value = {};

		input_value.float_value = 0.0f;

		input.create(input_value, "Smooth", NodePropertyType::FLOAT,
								 NodePropertySource::OUTPUT_UNIFORM);
		node_inputs.push_back(input);

		output = {};

		output.create("Quadratic", NodePropertyType::FLOAT);
		node_outputs.push_back(output);

		output = {};

		output.create("Linear", NodePropertyType::FLOAT);
		node_outputs.push_back(output);

		output = {};

		output.create("Constant", NodePropertyType::FLOAT);
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

    input.create(input_value, "texCoord", NodePropertyType::VECTOR2,
                 NodePropertySource::VS_OUT);
    node_inputs.push_back(input);

    output = {};

    output.create("UV", NodePropertyType::VECTOR3);
    node_outputs.push_back(output);
  } break;
  case NodeType::PRINCIPLED_BSDF: {
    node_name = "Principled BSDF";

    NodeInput input;
    NodeOutput output;
    NodePropertyValue input_value;

    input = {};
    input_value = {};

    input_value.vector4_value = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

    input.create(input_value, "Base Color", NodePropertyType::VECTOR4,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.float_value = 1.0f;

    input.create(input_value, "Alpha", NodePropertyType::FLOAT,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    output = {};

    output.create("BSDF", NodePropertyType::VECTOR4);
    node_outputs.push_back(output);
  } break;
  case NodeType::MIX: {
    node_name = "Mix";

    NodeInput input;
    NodeOutput output;
    NodePropertyValue input_value;

    input = {};
    input_value = {};

    input_value.enum_value = 0;

    input.create(input_value, "Data Type", NodePropertyType::ENUM,
                 NodePropertySource::INPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.float_value = 0.5f;

    input.create(input_value, "Factor", NodePropertyType::FLOAT,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.float_value = 0.0f;

    input.create(input_value, "A", NodePropertyType::FLOAT,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.float_value = 0.0f;

    input.create(input_value, "B", NodePropertyType::FLOAT,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.vector3_value = glm::vec3(0.0f);

    input.create(input_value, "A", NodePropertyType::VECTOR3,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.vector3_value = glm::vec3(0.0f);

    input.create(input_value, "B", NodePropertyType::VECTOR3,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.vector4_value = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    input.create(input_value, "A", NodePropertyType::VECTOR4,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    input = {};
    input_value = {};

    input_value.vector4_value = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    input.create(input_value, "B", NodePropertyType::VECTOR4,
                 NodePropertySource::OUTPUT_UNIFORM);
    node_inputs.push_back(input);

    output = {};

    output.create("Result", NodePropertyType::FLOAT);
    node_outputs.push_back(output);

    output = {};

    output.create("Result", NodePropertyType::VECTOR3);
    node_outputs.push_back(output);

    output = {};

    output.create("Result", NodePropertyType::VECTOR4);
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

bool NodeInput::useUniform(Material *material) {
  return enabled && (source == NodePropertySource::INPUT_UNIFORM ||
                     (source == NodePropertySource::OUTPUT_UNIFORM &&
                      (!link || !link->output(material)->enabled)));
}
