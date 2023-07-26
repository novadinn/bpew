#ifndef SPACE_SHADING_H
#define SPACE_SHADING_H

#include "../ecs/entity.h"
#include "../nodes/node.h"
#include "event_receiver.h"

struct SpaceShadingData {
  char unused;

  void createNode(Entity selected_enity, NodeType type);
};

EventReceiver *createSpaceShadingReceiver();

void onCreateSpaceShading(EditorContext *ctx);
void onDestroySpaceShading(EditorContext *ctx);

void onDrawUISpaceShading(EditorContext *ctx);

#endif // SPACE_SHADING_H
