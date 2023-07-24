#ifndef SCENE_SERIALIZER_H
#define SCENE_SERIALIZER_H

#include "scene.h"
#include "../graphics/material.h"

#include <string>
#include <yaml-cpp/yaml.h>

struct SceneSerializer {
  static void serialize(Scene *scene, const std::string &filepath);
  static bool deserialize(Scene *scene, const std::string &filepath);

private:
  static void serializeEntity(YAML::Emitter &out, Entity entity);
	static void serializeMaterial(YAML::Emitter &out, Material &material);
	static void deserializeMaterial(YAML::Node &material_data);
};

#endif // SCENE_SERIALIZER_H
