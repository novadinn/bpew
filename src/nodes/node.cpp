#include "node.h"

void Node::create(std::string node_name, NodeType node_type,
		  std::vector<NodeProperty> node_inputs,
		  std::vector<NodeProperty> node_outputs) {
  
  name = node_name;
  inputs = node_inputs;
  outputs = node_outputs;
  type = node_type;
}
