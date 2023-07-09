#ifndef NODE_LINK_H
#define NODE_LINK_H

#include "../core/id.h"
#include <sstream>

struct NodeInput;
struct NodeOutput;
struct Node;

struct NodeLink {
    void create(uint prop_output, uint prop_input,
		uint out_node, uint in_node);

    uint output_node;
    uint input_node;
    
    ID id;    
    uint output;
    uint input;    
};

#endif // NODE_LINK_H
