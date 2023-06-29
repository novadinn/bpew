#ifndef NODE_LINK_H
#define NODE_LINK_H

#include "../core/id.h"

struct NodeInput;
struct NodeOutput;

struct NodeLink {
    void create(NodeOutput* prop_output, NodeInput* prop_input,
		uint out_node, uint in_node);

    uint output_node;
    uint input_node;
    
    ID id;    
    NodeOutput* output;
    NodeInput* input;    
};

#endif // NODE_LINK_H
