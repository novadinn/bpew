#include "material_manager.h"

std::vector<Material> MaterialManager::materials;
Material MaterialManager::default_material;

Material *MaterialManager::getMaterial(uint index) {
  if (validMaterialIndex(index)) {
    return &materials[index];
  }

  return nullptr;
}

Material *MaterialManager::getMaterialOrDefault(uint index) {
  Material *material = getMaterial(index);

  if (!material || !material->shader_container->compiled) {
    return &default_material;
  }

  return material;
}

void MaterialManager::addMaterial(std::string name) {
  Material material;
  material.createDefault();
  material.name = name;

  materials.push_back(material);
}

bool MaterialManager::validMaterialIndex(uint index) {
  return index >= 0 && index < materials.size();
}

void MaterialManager::removeMaterial(uint index) {
  if (validMaterialIndex(index)) {
    materials.erase(materials.begin() + index);
  }
}
