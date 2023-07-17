#ifndef NODE_LINK_H
#define NODE_LINK_H

#include "../core/id.h"
#include <sstream>

struct NodeInput;
struct NodeOutput;
struct Node;
struct Material;

struct NodeLink {
    void create(uint prop_output_index, uint prop_input_index,
		uint out_node_index, uint in_node_index);

    uint output_node_index;
    uint input_node_index;
    
    ID id;    
    uint output_index;
    uint input_index;

    NodeInput* input(Material* material);
    NodeOutput* output(Material* material);

    Node* inputNode(Material* material);
    Node* outputNode(Material* material);
};

#endif // NODE_LINK_H
