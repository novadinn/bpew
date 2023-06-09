#ifndef NODE_LINK_H
#define NODE_LINK_H

#include "../core/id.h"

struct NodeProperty;

struct NodeLink {
  void create(NodeProperty* prop_output, NodeProperty* prop_input);
  
  ID id;
  NodeProperty* output;
  NodeProperty* input;
};

#endif // NODE_LINK_H
