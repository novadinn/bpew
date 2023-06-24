#ifndef NODE_LINK_H
#define NODE_LINK_H

#include "../core/id.h"

struct NodeProperty;
struct NodeInput;

struct NodeLink {
    void create(NodeProperty* prop_output, NodeProperty* prop_input,
		uint out_node, uint in_node);

    uint output_node;
    uint input_node;
    
  ID id;    
  NodeProperty* output;
  NodeProperty* input;    
};

#endif // NODE_LINK_H
