#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "../../graphics/model.h"
#include "../../graphics/material.h"

#include <vector>

struct MeshComponent {

  bool hasMaterials() const {
    return materials.size() > 0;
  }
  
  Model model;
  std::vector<Material> materials;
  int active_material_index = 0;
};

#endif // MESH_COMPONENT_H
