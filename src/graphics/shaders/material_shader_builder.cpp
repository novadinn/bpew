#include "material_shader_builder.h"

#include "../../core/log.h"
#include "../material.h"
#include "infos/material_shader_info.h"
#include "shader_container.h"
#include "shader_create_info.h"

std::map<std::string, ShaderContainer> MaterialShaderBuilder::shaders;
std::map<Material *, std::vector<int>> MaterialShaderBuilder::ids_buf;

bool MaterialShaderBuilder::buildMaterialShader(Material &material) {
  Sha new_sha = generateMaterialSha(material);

  if (material.shader_container &&
      material.shader_container->sha.sha.size() != 0 &&
      new_sha.sha == material.shader_container->sha.sha) {
    return true;
  }

  if (getShaderContainer(new_sha.sha)) {
    ShaderContainer &shader = shaders[new_sha.sha];
    increaseUsage(shader);
    if (material.shader_container) {
      decreaseUsage(new_sha.sha, (*material.shader_container));
    }
    material.shader_container = &shader;
    return true;
  }

  ShaderCreateInfo create_info = material_shader_create_info;

  std::stringstream ss;

  buildNodeTree(ss, create_info, material);
  create_info.define("USE_PREVIEW_SHADER");

  ShaderContainer container;
  container.material_functions = ss.str();
  container.create_info = create_info;
  container.users = 1;
  container.sha = new_sha;
  const auto [it, suc] = shaders.insert({new_sha.sha, container});
  material.shader_container = &it->second;

  return buildShaderFromShaderContainer(material.shader_container);
}

bool MaterialShaderBuilder::buildShaderFromShaderContainer(
    ShaderContainer *shader_container) {
  ShaderCreateInfo &create_info = shader_container->create_info;
  Shader &shader = shader_container->shader;

  std::stringstream ss;

  buildDefines(ss, create_info);

  std::stringstream vs, fs, gs;
  std::string buf;
  const char *base_dir = "datafiles/shaders/";

  char build_result = 0x0;
  if (create_info.info.vertex_source.size() > 0) {
    vs << ss.str();
    buildVertexShaderDefines(vs, create_info);
    std::string path(base_dir);
    path.append(create_info.info.vertex_source.c_str());
    build_result |= Utils::readFile(path.c_str(), buf);
    vs << buf;
  }
  if (create_info.info.fragment_source.size() > 0) {
    fs << ss.str();
    buildFragmentShaderDefines(fs, create_info);
    fs << shader_container->material_functions;
    std::string path(base_dir);
    path.append(create_info.info.fragment_source.c_str());
    build_result |= Utils::readFile(path.c_str(), buf) << 1;
    fs << buf;
  }
  if (create_info.info.geometry_source.size() > 0) {
    gs << ss.str();
    buildGeometryShaderDefines(gs, create_info);
    std::string path(base_dir);
    path.append(create_info.info.geometry_source.c_str());
    build_result |= Utils::readFile(path.c_str(), buf) << 2;
    gs << buf;
  }

  if (build_result == 0x7) {
    shader.destroy();

    if (!shader.createFromSource(vs.str().c_str(), fs.str().c_str(),
                                 gs.str().c_str())) {
      LOG_ERROR("Failed to build shader\n");
      return false;
    }
  } else if (build_result == 0x3) {
    shader.destroy();

    if (!shader.createFromSource(vs.str().c_str(), fs.str().c_str())) {
      LOG_ERROR("Failed to build shader\n");
      return false;
    }
  } else {
    LOG_ERROR("Failed to build shader\n");
    return false;
  }

  shader_container->compiled = true;

  return true;
}

bool MaterialShaderBuilder::buildMaterialRenderedShader(Material &material,
                                                        uint num_spot_lights,
                                                        uint num_point_lights,
                                                        uint num_dir_lights) {
  Sha new_sha = generateMaterialRenderedSha(material, num_spot_lights,
                                            num_point_lights, num_dir_lights);

  if (material.shader_container &&
      material.shader_container->sha.sha.size() != 0 &&
      new_sha.sha == material.shader_container->sha.sha) {
    return true;
  }

  if (shaders.contains(new_sha.sha)) {
    ShaderContainer &shader = shaders[new_sha.sha];
    increaseUsage(shader);
    if (material.shader_container) {
      decreaseUsage(new_sha.sha, (*material.shader_container));
    }
    material.shader_container = &shader;
    return true;
  }

  ShaderCreateInfo create_info = material_shader_create_info;

  std::stringstream ss;

  buildNodeTree(ss, create_info, material);

  create_info.dep("lights_lib.glsl");
  create_info.define("USE_RENDERED_SHADER");
  create_info.uniform(ShaderType::INT, "num_dir_lights");
  create_info.uniform(ShaderType::INT, "num_spot_lights");
  create_info.uniform(ShaderType::INT, "num_point_lights");
  if (num_spot_lights > 0) {
    create_info.define("CAPACITY_SPOT_LIGHTS", std::to_string(num_spot_lights));
    create_info.uniformArray("SpotLight", "spotLights", num_spot_lights);
  }
  if (num_point_lights > 0) {
    create_info.define("CAPACITY_POINT_LIGHTS",
                       std::to_string(num_point_lights));
    create_info.uniformArray("PointLight", "pointLights", num_point_lights);
  }
  if (num_dir_lights > 0) {
    create_info.define("CAPACITY_DIR_LIGHTS", std::to_string(num_dir_lights));
    create_info.uniformArray("DirectionalLight", "dirLights", num_dir_lights);
  }

  ShaderContainer container;
  container.material_functions = ss.str();
  container.create_info = create_info;
  container.users = 1;
  container.sha = new_sha;
  const auto [it, suc] = shaders.insert({new_sha.sha, container});
  material.shader_container = &it->second;

  return buildShaderFromShaderContainer(material.shader_container);
}

void MaterialShaderBuilder::buildNodeUniforms(ShaderCreateInfo &info,
                                              Node *node, Material *material) {
  for (auto &input : node->inputs) {
    if (input.enabled &&
        (input.source == NodePropertySource::INPUT_UNIFORM ||
         input.source == NodePropertySource::UNIFORM ||
         (input.source == NodePropertySource::OUTPUT_UNIFORM &&
          (!input.link || !input.link->output(material)->enabled)))) {
      buildNodeUniform(info, node, input);
    }
  }
}

ShaderContainer *MaterialShaderBuilder::getShaderContainer(std::string &hash) {
  if (!shaders.contains(hash)) {
    return nullptr;
  }

  return &shaders[hash];
}

void MaterialShaderBuilder::buildNodeTree(std::stringstream &ss,
                                          ShaderCreateInfo &create_info,
                                          Material &material) {
  generateMaterialIds(material);

  for (auto &node : material.nodes) {
    if (node.type == NodeType::MATERIAL_OUTPUT) {
      continue;
    }

    const char *node_src = getNodeSource(node.type);

    create_info.dep(node_src);
  }

  for (auto &node : material.nodes) {
    if (node.type == NodeType::MATERIAL_OUTPUT) {
      continue;
    }
    buildNodeUniforms(create_info, &node, &material);
  }

  const Node *out = nullptr;

  for (auto &node : material.nodes) {
    if (node.type == NodeType::MATERIAL_OUTPUT) {
      out = &node;
      break;
    }
  }

  if (!out) {
    LOG_ERROR("WARNING: material \"%s\" output not found\n",
              material.name.c_str());
  }

  const NodeInput *surface = nullptr;
  const NodeInput *volume = nullptr;
  const NodeInput *displacement = nullptr;

  if (out) {
    for (auto &input : out->inputs) {
      if (input.id.name == "Surface") {
        surface = &input;
      }
      if (input.id.name == "Volume") {
        volume = &input;
      }
      if (input.id.name == "Displacement") {
        displacement = &input;
      }
    }
  }

  ss << "vec4 surface() {\n";
  if (surface == nullptr || !surface->link) {
    ss << "return vec4(0.5, 0.5, 0.5, 1.0);\n";
  } else {
    Node *surface_node = surface->link->outputNode(&material);
    buildNode(ss, surface_node, material);
    ss << "return output_" << surface->link->output(&material)->id.id << ";\n";
  }
  ss << "}\n";

  revertMaterialIds(material);
}

void MaterialShaderBuilder::buildNodeUniform(ShaderCreateInfo &info, Node *node,
                                             const NodeInput &prop) {
  std::string name = std::string("input_") + std::to_string(prop.id.id);

  ShaderType type = toType(prop.type);

  info.uniform(type, name);
}

ShaderType MaterialShaderBuilder::toType(NodePropertyType type) {
  switch (type) {
  case NodePropertyType::VECTOR4:
    return ShaderType::VEC4;
  case NodePropertyType::VECTOR3:
    return ShaderType::VEC3;
  case NodePropertyType::VECTOR2:
    return ShaderType::VEC2;
  case NodePropertyType::FLOAT:
    return ShaderType::FLOAT;
  case NodePropertyType::INT:
  case NodePropertyType::ENUM:
    return ShaderType::INT;
  case NodePropertyType::TEXTURE:
    return ShaderType::SAMPLER_2D;
  default:
    LOG_ERROR("Unhandled node property type\n");
    return ShaderType::INT;
  }
}

void MaterialShaderBuilder::buildNode(std::stringstream &ss, Node *node,
                                      Material &material) {
  for (auto &input : node->inputs) {
    if (input.link && input.enabled && input.link->output(&material)->enabled) {
      buildNode(ss, input.link->outputNode(&material), material);
    }

    if (input.source == NodePropertySource::OUTPUT &&
            (!input.link || !input.enabled) ||
        (input.source == NodePropertySource::OUTPUT_UNIFORM &&
         !input.enabled)) {
      ss << fromType(input.type) << " input_" << input.id.id << ";\n";
    }
  }

  for (int i = 0; i < node->outputs.size(); ++i) {
    auto &output = node->outputs[i];
    ss << fromType(output.type) << " output_" << output.id.id << ";\n";
  }

  ss << getNodeName(node->type) << "(";
  for (int i = 0; i < node->inputs.size(); ++i) {
    auto &input = node->inputs[i];

    switch (input.source) {
    case INPUT_UNIFORM:
      ss << "input_" << input.id.id;
      break;
    case OUTPUT:
    case OUTPUT_UNIFORM:
      if (input.link && input.enabled && input.link->output(&material)->enabled)
        ss << "output_" << input.link->output(&material)->id.id;
      else
        ss << "input_" << input.id.id;
      break;
    case VS_OUT:
      ss << "vs_inout." << input.id.name;
      break;
    case UNIFORM:
      ss << input.id.name;
      break;
    }

    ss << ", ";
  }
  for (int i = 0; i < node->outputs.size(); ++i) {
    auto &output = node->outputs[i];

    ss << "output_" << output.id.id;

    if (i != node->outputs.size() - 1) {
      ss << ", ";
    }
  }

  ss << ");\n";
}

const char *MaterialShaderBuilder::getNodeName(NodeType type) {
  const char *src = "";
  switch (type) {
  case NodeType::RGB:
    src = "node_rgb";
    break;
  case NodeType::BEVEL:
    src = "node_bevel";
    break;
  case NodeType::BRICK_TEXTURE:
    src = "node_tex_brick";
    break;
  case NodeType::CHECKER_TEXTURE:
    src = "node_tex_checker";
    break;
  case NodeType::IMAGE_TEXTURE:
    src = "node_image_texture";
    break;
  case NodeType::MAGIC_TEXTURE:
    src = "node_tex_magic";
    break;
  case NodeType::MUSGRAVE_TEXTURE:
    src = "node_tex_musgrave";
    break;
  case NodeType::NOISE_TEXTURE:
    src = "node_noise_texture";
    break;
  case NodeType::BRIGHTNESS_CONTRAST:
    src = "node_brightness_contrast";
    break;
  case NodeType::GAMMA:
    src = "node_gamma";
    break;
  case NodeType::INVERT:
    src = "node_invert";
    break;
  case NodeType::LIGHT_FALLOFF:
    src = "node_light_falloff";
    break;
  case NodeType::TEXTURE_COORDINATE:
    src = "node_texture_coordinate";
    break;
  case NodeType::PRINCIPLED_BSDF:
    src = "node_principled_bsdf";
    break;
  case NodeType::MIX:
    src = "node_mix";
    break;
  default:
    LOG_ERROR("Unknown node type: %d\n", type);
  }

  return src;
}

const char *MaterialShaderBuilder::getNodeSource(NodeType type) {
  const char *src = "";
  switch (type) {
  case NodeType::RGB:
    src = "node_rgb.glsl";
    break;
  case NodeType::BEVEL:
    src = "node_bevel.glsl";
    break;
  case NodeType::BRICK_TEXTURE:
    src = "node_tex_brick.glsl";
    break;
  case NodeType::CHECKER_TEXTURE:
    src = "node_tex_checker.glsl";
    break;
  case NodeType::IMAGE_TEXTURE:
    src = "node_image_texture.glsl";
    break;
  case NodeType::MAGIC_TEXTURE:
    src = "node_tex_magic.glsl";
    break;
  case NodeType::MUSGRAVE_TEXTURE:
    src = "node_tex_musgrave.glsl";
    break;
  case NodeType::NOISE_TEXTURE:
    src = "node_tex_noise.glsl";
    break;
  case NodeType::BRIGHTNESS_CONTRAST:
    src = "node_brightness_contrast.glsl";
    break;
  case NodeType::GAMMA:
    src = "node_gamma.glsl";
    break;
  case NodeType::INVERT:
    src = "node_invert.glsl";
    break;
  case NodeType::LIGHT_FALLOFF:
    src = "node_light_falloff.glsl";
    break;
  case NodeType::TEXTURE_COORDINATE:
    src = "node_texture_coordinate.glsl";
    break;
  case NodeType::PRINCIPLED_BSDF:
    src = "node_principled_bsdf.glsl";
    break;
  case NodeType::MIX:
    src = "node_mix.glsl";
    break;
  default:
    LOG_ERROR("Unknown node type: %d\n", type);
  }

  return src;
}

const char *MaterialShaderBuilder::fromType(NodePropertyType type) {
  switch (type) {
  case NodePropertyType::VECTOR4:
    return "vec4";
  case NodePropertyType::VECTOR3:
    return "vec3";
  case NodePropertyType::VECTOR2:
    return "vec2";
  case NodePropertyType::FLOAT:
    return "float";
  case NodePropertyType::INT:
  case NodePropertyType::ENUM:
    return "int";
  case NodePropertyType::TEXTURE:
    return "sampler2D";
  default:
    LOG_ERROR("Unknown node property type: %d\n", type);
    return "";
  }
}

Sha MaterialShaderBuilder::generateMaterialSha(Material &material) {
  Sha sha;

  std::stringstream material_info;

  // TODO: sort nodes
  for (auto &node : material.nodes) {
    material_info << (int)node.type;

    for (auto &input : node.inputs) {
      material_info << input.enabled;
      if (input.link) {
        material_info << input.link->output(&material)->enabled;
        material_info << input.link->output_index;
        material_info << (int)input.link->outputNode(&material)->type;
        material_info << (int)input.link->inputNode(&material)->type;
      }
    }
  }

  std::string value = material_info.str();
  sha.create(value.c_str(), value.size());
  return sha;
}

Sha MaterialShaderBuilder::generateMaterialRenderedSha(Material &material,
                                                       uint num_spot_lights,
                                                       uint num_point_lights,
                                                       uint num_dir_lights) {
  Sha sha = generateMaterialSha(material);

  std::stringstream ss;
  ss << sha.sha << num_spot_lights << num_point_lights << num_dir_lights;

  std::string value = ss.str();

  sha.create(value.c_str(), value.size());
  return sha;
}

void MaterialShaderBuilder::decreaseUsage(std::string &key,
                                          ShaderContainer &container) {
  container.users--;
  if (container.users == 0) {
    // TODO: clear unused shaders
    // if(container.compiled) {
    //     container.shader.destroy();
    // }

    // shaders.erase(key);
  }
}

void MaterialShaderBuilder::increaseUsage(ShaderContainer &container) {
  container.users++;
}

void MaterialShaderBuilder::generateMaterialIds(Material &material) {
  std::vector<int> prev_ids;
  int id = 0;

  // generate ids
  // NOTE: We don't need to regenerate shader when only ids have changed
  for (auto &node : material.nodes) {
    prev_ids.push_back(node.id.id);
    node.id.id = id++;

    for (auto &input : node.inputs) {
      prev_ids.push_back(input.id.id);
      input.id.id = id++;
    }

    for (auto &output : node.outputs) {
      prev_ids.push_back(output.id.id);
      output.id.id = id++;
    }
  }

  ids_buf.insert({&material, prev_ids});
}

void MaterialShaderBuilder::revertMaterialIds(Material &material) {
  if (!ids_buf.contains(&material))
    return;

  std::vector<int> &prev_ids = ids_buf[&material];

  for (auto &node : material.nodes) {
    node.id.id = prev_ids[node.id.id];

    for (auto &input : node.inputs) {
      input.id.id = prev_ids[input.id.id];
    }

    for (auto &output : node.outputs) {
      output.id.id = prev_ids[output.id.id];
    }
  }

  ids_buf.erase(&material);
}
