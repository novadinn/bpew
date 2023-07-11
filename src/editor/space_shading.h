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

void onDrawUIBeginSpaceShading(EditorContext *ctx);
void onDrawUISpaceShading(EditorContext *ctx);
void onDrawUIEndSpaceShading(EditorContext *ctx);

struct SpaceShading {
  void begin();

  void drawHeader(Entity selected_entity);
  void drawNodeEditor(Entity selected_entity);

  void end();

private:
  void drawContextMenu(Entity selected_entity);
  void updateLinks(Entity selected_entity);
  void createNode(Entity selected_enity, NodeType type);
};

#endif // SPACE_SHADING_H
