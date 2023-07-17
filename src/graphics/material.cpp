#include "material.h"

void Material::createDefault() {
    Node bsdf;
    bsdf.create(NodeType::PRINCIPLED_BSDF);    

    Node output;    
    output.create(NodeType::MATERIAL_OUTPUT);

    NodeLink *bsdf_output_link = new NodeLink();
    bsdf_output_link->create(0, 0, 0, 1);

    bsdf.outputs[0].links.push_back(*bsdf_output_link);
    output.inputs[0].link = bsdf_output_link;

    nodes.push_back(bsdf);
    nodes.push_back(output);
}
