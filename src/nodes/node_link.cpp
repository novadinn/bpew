#include "node_link.h"

#include "node.h"

void NodeLink::create(NodeOutput* prop_output, NodeInput* prop_input,
		      uint out_node, uint in_node) {
  // TODO: use more reasonable names
  id.create(prop_output->id.name + prop_input->id.name);
  output = prop_output;
  input = prop_input;
  output_node = out_node;
  input_node = in_node;
}
