#ifndef NODE_H
#define NODE_H

#include "../core/id.h"
#include "node_property.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

enum NodeType {
  // Input
  RGB,
  // Output
  MATERIAL_OUTPUT
  // Shader
  // Texture
  // Color
  // Vector
  // Converter
};

struct Node {
  void create(NodeType node_type);

  ID id;
  std::vector<NodeProperty> inputs;
  std::vector<NodeProperty> outputs;

  NodeType type;
};

#endif // NODE_H
