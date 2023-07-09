#include "node_link.h"

#include "node.h"

void NodeLink::create(uint prop_output, uint prop_input,
		      uint out_node, uint in_node) {    
    std::stringstream name;
    name << "link" << out_node << in_node << prop_output << prop_input;
    id.create(name.str());
    output = prop_output;
    input = prop_input;
    output_node = out_node;
    input_node = in_node;
}
