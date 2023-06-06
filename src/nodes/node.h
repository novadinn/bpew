#ifndef NODE_H
#define NODE_H

#include "node_property.h"
#include "../core/utils.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

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
  
  std::string name;
  std::vector<NodeProperty> inputs;
  std::vector<NodeProperty> outputs;
  NodeType type;
  uint64 id;
};

#endif // NODE_H
