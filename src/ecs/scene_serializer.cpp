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

            mc.meshes.push_back(mesh);
          }
        }
      }
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

    /* TODO: export materials */

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

        out << YAML::EndMap;
      }

      out << YAML::EndSeq;
    }

    out << YAML::EndMap;
  }

  out << YAML::EndMap;
}