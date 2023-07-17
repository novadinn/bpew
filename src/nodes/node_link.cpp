#include "node_link.h"
#include "../graphics/material.h"
#include "node.h"

void NodeLink::create(uint prop_output_index, uint prop_input_index,
		      uint out_node_index, uint in_node_index) {    
    std::stringstream name;
    name << "link" << out_node_index << in_node_index << prop_output_index << prop_input_index;
    id.create(name.str());
    output_index = prop_output_index;
    input_index = prop_input_index;
    output_node_index = out_node_index;
    input_node_index = in_node_index;
}

NodeInput* NodeLink::input(Material* material) {
    Node *input_node = inputNode(material);
    
    if(!input_node) {
	return nullptr;
    }

    if(input_index >= 0 && input_index < input_node->inputs.size()) {
	return &input_node->inputs[input_index];
    }

    return nullptr;
}
NodeOutput* NodeLink::output(Material* material) {
    Node *output_node = outputNode(material);
    
    if(!output_node) {
	return nullptr;
    }

    if(output_index >= 0 && output_index < output_node->outputs.size()) {
	return &output_node->outputs[output_index];
    }

    return nullptr;
}

Node* NodeLink::inputNode(Material* material) {
    if(input_node_index >= 0 && input_node_index < material->nodes.size()) {
	return &material->nodes[input_node_index];
    }

    return nullptr;
}
Node* NodeLink::outputNode(Material* material) {
    if(output_node_index >= 0 && output_node_index < material->nodes.size()) {
	return &material->nodes[output_node_index];
    }

    return nullptr;
}
