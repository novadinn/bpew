#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "../../graphics/model.h"
#include "../../graphics/material.h"

#include <vector>

struct MeshComponent {

  bool hasMaterials() const {
    return materials.size() > 0;
  }
  bool validMaterialIndex() const {
    return active_material_index >= 0 && active_material_index < materials.size();
  }
  Material* getActiveMaterial() {
    if(!validMaterialIndex()) {
      return nullptr;
    }

    return &materials[active_material_index];
  }
  
  Model model;
  std::vector<Material> materials;
    Material default_material;
  int active_material_index = -1;
};

#endif // MESH_COMPONENT_H
