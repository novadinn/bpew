#include "scene_serializer.h"

#include "components/camera_component.h"
#include "components/light_component.h"
#include "components/mesh_component.h"
#include "components/tag_component.h"
#include "components/transform_component.h"
#include "components/uuid_component.h"
#include "entity.h"

#include <fstream>
#include <glm/glm.hpp>

namespace YAML {

template <> struct convert<glm::vec2> {
  static Node encode(const glm::vec2 &rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.SetStyle(EmitterStyle::Flow);
    return node;
  }

  static bool decode(const Node &node, glm::vec2 &rhs) {
    if (!node.IsSequence() || node.size() != 2)
      return false;

    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    return true;
  }
};

template <> struct convert<glm::vec3> {
  static Node encode(const glm::vec3 &rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    node.SetStyle(EmitterStyle::Flow);
    return node;
  }

  static bool decode(const Node &node, glm::vec3 &rhs) {
    if (!node.IsSequence() || node.size() != 3)
      return false;

    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    rhs.z = node[2].as<float>();
    return true;
  }
};

template <> struct convert<glm::vec4> {
  static Node encode(const glm::vec4 &rhs) {
    Node node;
    node.push_back(rhs.x);
    node.push_back(rhs.y);
    node.push_back(rhs.z);
    node.push_back(rhs.w);
    node.SetStyle(EmitterStyle::Flow);
    return node;
  }

  static bool decode(const Node &node, glm::vec4 &rhs) {
    if (!node.IsSequence() || node.size() != 4)
      return false;

    rhs.x = node[0].as<float>();
    rhs.y = node[1].as<float>();
    rhs.z = node[2].as<float>();
    rhs.w = node[3].as<float>();
    return true;
  }
};

template <> struct convert<VertexAttribute> {
  static Node encode(const VertexAttribute &rhs) {
    Node node;
    node.push_back(rhs.getSize());
    node.push_back(rhs.getCount());
    node.push_back(rhs.isNormalized());
    node.SetStyle(EmitterStyle::Flow);
    return node;
  }

  static bool decode(const Node &node, VertexAttribute &rhs) {
    if (!node.IsSequence() || node.size() != 3)
      return false;

    rhs.size = node[0].as<size_t>();
    rhs.count = node[1].as<uint>();
    rhs.normalized = node[2].as<GLenum>();
    return true;
  }
};

} // namespace YAML

YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec2 &v) {
  out << YAML::Flow;
  out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
  return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec3 &v) {
  out << YAML::Flow;
  out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
  return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec4 &v) {
  out << YAML::Flow;
  out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
  return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const VertexAttribute &va) {
  out << YAML::Flow;
  out << YAML::BeginSeq << va.getSize() << va.getCount() << va.isNormalized()
      << YAML::EndSeq;
  return out;
}

void SceneSerializer::serialize(Scene *scene, const std::string &filepath) {
  YAML::Emitter out;
  out << YAML::BeginMap;
  out << YAML::Key << "Scene" << YAML::Value << "Untitled";
  out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
  entt::registry &registry = scene->getEntityRegistry();
  registry.each([&](auto entityID) {
    Entity entity;
    entity.create(entityID, scene);
    if (!entity)
      return;

    serializeEntity(out, entity);
  });
  out << YAML::EndSeq;

	out << YAML::Key << "ImageTextures" << YAML::Value << YAML::BeginSeq;
	for (auto it = Texture2D::loaded_textures.begin(); it != Texture2D::loaded_textures.end(); ++it) {		
		out << YAML::BeginMap;
		out << YAML::Key << "Texture" << YAML::Value << it->second;

		out << YAML::Key << "Path" << YAML::Value << it->first;
		
		out << YAML::EndMap;
	}
	out << YAML::EndSeq;

	out << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;
	for (Material &material : MaterialManager::materials) {
		serializeMaterial(out, material);
	}
	out << YAML::EndSeq;
	
  out << YAML::EndMap;

  std::ofstream fout(filepath);
  fout << out.c_str();
}

bool SceneSerializer::deserialize(Scene *scene, const std::string &filepath) {

  YAML::Node data;
  try {
    data = YAML::LoadFile(filepath);
  } catch (YAML::ParserException e) {
    LOG_ERROR("Failed to load .hazel file %s\n", filepath.c_str());

    return false;
  }

  if (!data["Scene"])
    return false;

  std::string scene_name = data["Scene"].as<std::string>();

  auto entities = data["Entities"];
  if (entities) {
    for (auto entity : entities) {
      /* those 3 components always exists, dont need check for them. */

      ASSERT(entity["TagComponent"]);
      ASSERT(entity["TransformComponent"]);
      ASSERT(entity["Entity"]);

      std::string name;
      auto tag = entity["TagComponent"];
      name = tag["Tag"].as<std::string>();

      uint64 uuid = entity["Entity"].as<uint64>();
      UUID id;
      id.createFromUUID(uuid);
      Entity deserialized_entity = scene->createEntityFromUUID(id, name);

      auto transform = entity["TransformComponent"];
      auto &tc = deserialized_entity.getComponent<TransformComponent>();
      tc.position = transform["Position"].as<glm::vec3>();
      tc.scale = transform["Scale"].as<glm::vec3>();
      tc.rotation = transform["Rotation"].as<glm::vec3>();

      auto camera = entity["CameraComponent"];
      if (camera) {
        auto &cc = deserialized_entity.addComponent<CameraComponent>();

        cc.fov = camera["FOV"].as<float>();
        cc.near = camera["Near"].as<float>();
        cc.far = camera["Far"].as<float>();
        cc.focal_point = camera["Focal Point"].as<glm::vec3>();
        cc.aspect_ratio = camera["Aspect Ratio"].as<float>();
        cc.distance = camera["Distance"].as<float>();
        cc.viewport_width = camera["Viewport Width"].as<float>();
        cc.viewport_height = camera["Viewport Height"].as<float>();
      }

      auto light = entity["LightComponent"];
      if (light) {
        auto &lc = deserialized_entity.addComponent<LightComponent>();

        lc.type = (LightComponent::LightType)light["Type"].as<int>();
        lc.ambient = light["Ambient"].as<glm::vec3>();
        lc.diffuse = light["Diffuse"].as<glm::vec3>();
        lc.specular = light["Specular"].as<glm::vec3>();

        if (light["Cutoff"] && light["Outer Cutoff"]) {
          lc.properties.spot_light.cut_off = light["Cutoff"].as<float>();
          lc.properties.spot_light.outer_cut_off =
              light["Outer Cutoff"].as<float>();
        }
      }

      auto mesh = entity["MeshComponent"];
      if (mesh) {
        auto &mc = deserialized_entity.addComponent<MeshComponent>();

        auto mesh_fields = mesh["MeshFields"];
        if (mesh_fields) {
          for (auto field : mesh_fields) {
            std::vector<VertexAttribute> attribs =
                field["VertexAttributes"].as<std::vector<VertexAttribute>>();
            std::vector<float> vertices =
                field["Vertices"].as<std::vector<float>>();
            std::vector<uint> indices =
                field["Indices"].as<std::vector<uint>>();						

            Mesh mesh;
            mesh.attributes = attribs;
            mesh.vertices = vertices;
            mesh.indices = indices;
            mesh.generateVertexArray();

						auto material = field["Material"];
						if (material) {
							mesh.active_material_index = material.as<int>();
						}
						
            mc.meshes.push_back(mesh);
          }
        }

				auto materials_data = mesh["Materials"];
				if (materials_data) {
					std::vector<uint> materials = materials_data.as<std::vector<uint>>();

					mc.materials = materials;
				}
      }
    }
  }

	auto loaded_textures = data["ImageTextures"];
	if (loaded_textures) {
		for (auto texture_data : loaded_textures) {
			std::string path = texture_data["Path"].as<std::string>();

			Texture2D texture;
			texture.createFromFile(path.c_str());			
		}
	}

	auto materials = data["Materials"];
	if (materials) {
		for (auto material : materials) {
			deserializeMaterial(material);
		}
	}

  return true;
}

void SceneSerializer::serializeEntity(YAML::Emitter &out, Entity entity) {
  out << YAML::BeginMap;

  /* those 3 always exists */
  ASSERT(entity.hasComponent<UUIDComponent>());
  ASSERT(entity.hasComponent<TagComponent>());
  ASSERT(entity.hasComponent<TransformComponent>());

  UUIDComponent &uuid = entity.getComponent<UUIDComponent>();

  out << YAML::Key << "Entity" << YAML::Value << (uint64)uuid.id;

  out << YAML::Key << "TagComponent";
  out << YAML::BeginMap;

  std::string &tag = entity.getComponent<TagComponent>().tag;
  out << YAML::Key << "Tag" << YAML::Value << tag;

  out << YAML::EndMap;

  out << YAML::Key << "TransformComponent";
  out << YAML::BeginMap;

  auto &tc = entity.getComponent<TransformComponent>();
  out << YAML::Key << "Position" << YAML::Value << tc.position;
  out << YAML::Key << "Scale" << YAML::Value << tc.scale;
  out << YAML::Key << "Rotation" << YAML::Value << tc.rotation;

  out << YAML::EndMap;

  if (entity.hasComponent<CameraComponent>()) {
    out << YAML::Key << "CameraComponent";
    out << YAML::BeginMap;

    auto &camera = entity.getComponent<CameraComponent>();
    out << YAML::Key << "FOV" << YAML::Value << camera.fov;
    out << YAML::Key << "Near" << YAML::Value << camera.near;
    out << YAML::Key << "Far" << YAML::Value << camera.far;
    out << YAML::Key << "Focal Point" << YAML::Value << camera.focal_point;
    out << YAML::Key << "Aspect Ratio" << YAML::Value << camera.aspect_ratio;
    out << YAML::Key << "Distance" << YAML::Value << camera.distance;
    out << YAML::Key << "Viewport Width" << YAML::Value
        << camera.viewport_width;
    out << YAML::Key << "Viewport Height" << YAML::Value
        << camera.viewport_height;

    out << YAML::EndMap;
  }

  if (entity.hasComponent<LightComponent>()) {
    out << YAML::Key << "LightComponent";
    out << YAML::BeginMap;

    auto &light = entity.getComponent<LightComponent>();
    out << YAML::Key << "Type" << YAML::Value << (int)light.type;
    out << YAML::Key << "Ambient" << YAML::Value << light.ambient;
    out << YAML::Key << "Diffuse" << YAML::Value << light.diffuse;
    out << YAML::Key << "Specular" << YAML::Value << light.specular;
    switch (light.type) {
    case LightComponent::LightType::SPOT: {
      out << YAML::Key << "Cutoff" << YAML::Value
          << light.properties.spot_light.cut_off;
      out << YAML::Key << "Outer Cutoff" << YAML::Value
          << light.properties.spot_light.outer_cut_off;
    } break;
    };

    out << YAML::EndMap;
  }

  if (entity.hasComponent<MeshComponent>()) {
    out << YAML::Key << "MeshComponent";
    out << YAML::BeginMap;

    auto &mesh = entity.getComponent<MeshComponent>();
    if (mesh.meshes.size() > 0) {
      out << YAML::Key << "MeshFields" << YAML::Value;
      out << YAML::BeginSeq;

      for (int i = 0; i < mesh.meshes.size(); ++i) {
        out << YAML::BeginMap;
        out << YAML::Key << "Mesh" << YAML::Value << std::to_string(i);

        std::vector<VertexAttribute> &attributes = mesh.meshes[i].attributes;
        std::vector<float> &vertices = mesh.meshes[i].vertices;
        std::vector<uint> &indices = mesh.meshes[i].indices;

        out << YAML::Key << "VertexAttributes" << YAML::Value << attributes;
        out << YAML::Key << "Vertices" << YAML::Value << vertices;
        out << YAML::Key << "Indices" << YAML::Value << indices;

				if (MaterialManager::validMaterialIndex(mesh.meshes[i].active_material_index)) {
					out << YAML::Key << "Material" << YAML::Value << mesh.meshes[i].active_material_index;
				}				

        out << YAML::EndMap;
      }					 		 

      out << YAML::EndSeq;
    }

		if (mesh.materials.size() > 0) {
			out << YAML::Key << "Materials" << YAML::Value << mesh.materials;		 
		}

    out << YAML::EndMap;
  }

  out << YAML::EndMap;
}

void SceneSerializer::serializeMaterial(YAML::Emitter &out, Material &material) {
	out << YAML::BeginMap;
	out << YAML::Key << "Material" << YAML::Value << material.name;				 	

	if (material.nodes.size() > 0) {
		out << YAML::Key << "Nodes" << YAML::Value;
		out << YAML::BeginSeq;
						
		for (auto &node : material.nodes) {
			out << YAML::BeginMap;
			out << YAML::Key << "Node" << YAML::Value << node.id.id;
							
			out << YAML::Key << "Name" << YAML::Value << node.id.name;

			out << YAML::Key << "Type" << YAML::Value << (int)node.type;

			if (node.inputs.size() > 0) {
				out << YAML::Key << "Inputs" << YAML::Value;
				out << YAML::BeginSeq;

				for (auto &input : node.inputs) {
					out << YAML::BeginMap;									
					out << YAML::Key << "Input" << YAML::Value << input.id.id;

					out << YAML::Key << "Name" << YAML::Value << input.id.name;

					if (input.link) {
						out << YAML::Key << "Link";
						out << YAML::BeginMap;

						out << YAML::Key << "Id" << YAML::Value << input.link->id.id;
						out << YAML::Key << "Name" << YAML::Value << input.link->id.name;
						out << YAML::Key << "OutputNodeIndex" << YAML::Value << input.link->output_node_index;
						out << YAML::Key << "InputNodeIndex" << YAML::Value << input.link->input_node_index;
						out << YAML::Key << "OutputIndex" << YAML::Value << input.link->output_index;
						out << YAML::Key << "InputIndex" << YAML::Value << input.link->input_index;
										
						out << YAML::EndMap;
					}
					
					out << YAML::Key << "Type" << YAML::Value << (int)input.type;
					out << YAML::Key << "Source" << YAML::Value << (int)input.source;

					out << YAML::Key << "Value";
					switch (input.type) {
					case NodePropertyType::VECTOR4:
						out << input.value.vector4_value;
						break;
					case NodePropertyType::COLOR:
					case NodePropertyType::VECTOR3:
						out << input.value.vector3_value;
						break;
					case NodePropertyType::VECTOR2:
						out << input.value.vector2_value;
						break;
					case NodePropertyType::FLOAT:
						out << input.value.float_value;
						break;
					case NodePropertyType::INT:
						out << input.value.int_value;
						break;
					case NodePropertyType::ENUM:
						out << input.value.enum_value;
						break;									
					case NodePropertyType::TEXTURE:					
						out << input.value.texture_value;						
						break;
					default:
						LOG_ERROR("Unknown node input type: %d\n", (int)input.type);
					}

					out << YAML::Key << "Enabled" << YAML::Value << input.enabled;
									
					out << YAML::EndMap;
				}
								
				out << YAML::EndSeq;
			}

			if (node.outputs.size() > 0) {
				out << YAML::Key << "Outputs" << YAML::Value;
				out << YAML::BeginSeq;

				for (auto &output : node.outputs) {
					out << YAML::BeginMap;
					out << YAML::Key << "Output" << YAML::Value << output.id.id;

					out << YAML::Key << "Name" << YAML::Value << output.id.name;

					if (output.links.size() > 0) {
						out << YAML::Key << "Links" << YAML::Value;
						out << YAML::BeginSeq;

						for (auto &link : output.links) {
							out << YAML::BeginMap;
							out << YAML::Key << "Link" << YAML::Value << link.id.id;
											
							out << YAML::Key << "Name" << YAML::Value << link.id.name;
							out << YAML::Key << "OutputNodeIndex" << YAML::Value << link.output_node_index;
							out << YAML::Key << "InputNodeIndex" << YAML::Value << link.input_node_index;
							out << YAML::Key << "OutputIndex" << YAML::Value << link.output_index;
							out << YAML::Key << "InputIndex" << YAML::Value << link.input_index;
										
							out << YAML::EndMap;
						}
										
						out << YAML::EndSeq;
					}

					out << YAML::Key << "Type" << YAML::Value << (int)output.type;

					out << YAML::Key << "Enabled" << YAML::Value << output.enabled;
									
					out << YAML::EndMap;
				}								
								
				out << YAML::EndSeq;
			}							

			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
	}
					
	out << YAML::EndMap;
}

void SceneSerializer::deserializeMaterial(YAML::Node &material_data) {
	ASSERT(material_data["Material"]);

	Material material;

	material.name = material_data["Material"].as<std::string>();

	auto nodes = material_data["Nodes"];
	if (nodes) {
		for (auto node_data : nodes) {
			Node node;

			node.id.id = node_data["Node"].as<int>();
			node.id.name = node_data["Name"].as<std::string>();

			node.type = (NodeType)node_data["Type"].as<int>();

			auto inputs = node_data["Inputs"];
			if (inputs) {
				for (auto input_data : inputs) {
					NodeInput input;

					input.id.id = input_data["Input"].as<int>();
					input.id.name = input_data["Name"].as<std::string>();

					auto link_data = input_data["Link"];
					if (link_data) {
						NodeLink *link = new NodeLink();

						link->id.id = link_data["Id"].as<int>();
						link->id.name = link_data["Name"].as<std::string>();

						link->output_node_index = link_data["OutputNodeIndex"].as<uint>();
						link->input_node_index = link_data["InputNodeIndex"].as<uint>();

						link->output_index = link_data["OutputIndex"].as<uint>();
						link->input_index = link_data["InputIndex"].as<uint>();

						input.link = link;
					}

					input.type = (NodePropertyType)input_data["Type"].as<int>();
					input.source = (NodePropertySource)input_data["Source"].as<int>();
					auto value = input_data["Value"];
					switch(input.type) {
					case NodePropertyType::VECTOR4:
						input.value.vector4_value = value.as<glm::vec4>();
						break;
					case NodePropertyType::COLOR:
					case NodePropertyType::VECTOR3:
						input.value.vector3_value = value.as<glm::vec3>();
						break;
					case NodePropertyType::VECTOR2:
						input.value.vector2_value = value.as<glm::vec2>();
						break;
					case NodePropertyType::FLOAT:
						input.value.float_value = value.as<float>();
						break;
					case NodePropertyType::INT:
						input.value.int_value = value.as<int>();
						break;
					case NodePropertyType::ENUM:
						input.value.enum_value = value.as<int>();
						break;									
					case NodePropertyType::TEXTURE:						
						input.value.texture_value = value.as<uint>();
						break;
					default:
						LOG_ERROR("Unknown node input type: %d\n", (int)input.type);
					}

					input.enabled = input_data["Enabled"].as<bool>();

					node.inputs.push_back(input);
				}				
			}

			auto outputs = node_data["Outputs"];
			if (outputs) {
				for (auto output_data : outputs) {
					NodeOutput output;

					output.id.id = output_data["Output"].as<int>();
					output.id.name = output_data["Name"].as<std::string>();

					auto links = output_data["Links"];
					if (links) {
						for (auto link_data : links) {
							NodeLink link;

							link.id.id = link_data["Link"].as<int>();
							link.id.name = link_data["Name"].as<std::string>();

							link.output_node_index = link_data["OutputNodeIndex"].as<uint>();
							link.input_node_index = link_data["InputNodeIndex"].as<uint>();

							link.output_index = link_data["OutputIndex"].as<uint>();
							link.input_index = link_data["InputIndex"].as<uint>();

							output.links.push_back(link);
						}
					}

					output.type = (NodePropertyType)output_data["Type"].as<int>();					
					output.enabled = output_data["Enabled"].as<bool>();
					
					node.outputs.push_back(output);
				}
			}

			material.nodes.push_back(node);
		}
	}

	MaterialManager::materials.push_back(material);
}
