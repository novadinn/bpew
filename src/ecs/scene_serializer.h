#ifndef SCENE_SERIALIZER_H
#define SCENE_SERIALIZER_H

#include "../graphics/material.h"
#include "scene.h"

#include <string>
#include <yaml-cpp/yaml.h>

struct SceneSerializer {
  static void serialize(Scene *scene, const std::string &filepath);
  static bool deserialize(Scene *scene, const std::string &filepath);

private:
  static void serializeEntity(YAML::Emitter &out, Entity entity);
  static void serializeTexture2D(YAML::Emitter &out, Texture2D &texture);
  static void serializeMaterial(YAML::Emitter &out, Material &material);
  static void deserializeMaterial(YAML::Node &material_data);
};

#endif // SCENE_SERIALIZER_H
