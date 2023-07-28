#ifndef MESH_H
#define MESH_H

#include "../core/platform.h"
#include "material.h"
#include "material_manager.h"
#include "vertex_array.h"
#include "vertex_attribute.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct Mesh {
  void generateVertexArray() {
    va.create();
    va.bind();

    vb.create(&vertices[0], vertices.size() * sizeof(float));
    if (indices.size() > 0)
      ib.create(&indices[0], indices.size() * sizeof(uint));

    va.addVertexBuffer(vb, attributes);
    if (indices.size() > 0)
      va.setIndexBuffer(ib);
  }

  void destroy() {
    ib.destroy();
    vb.destroy();
    va.destroy();
  }

  int totalAttributesCount() const {
    int total_count = 0;
    for (int i = 0; i < attributes.size(); ++i) {
      total_count += attributes[i].getCount();
    }
    return total_count;
  }

  Material *getActiveMaterial() {
    return MaterialManager::getMaterialOrDefault(active_material_index);
  }

  int active_material_index = -1;

  // TODO: store a type of a vertex subdata in here (to identify
  // position/normal..)
  std::vector<VertexAttribute> attributes;
  std::vector<float> vertices;
  std::vector<uint> indices;

  VertexArray va;
  VertexBuffer vb;
  IndexBuffer ib;
};

#endif // MESH_H
