#ifndef SPACE_LAYOUT_H
#define SPACE_LAYOUT_H

#include "event_receiver.h"
#include "view3d_region.h"

#include <glm/glm.hpp>

struct SpaceLayoutData {
  View3dRegion *view3d_region;
  Entity hovered_entity;
};

EventReceiver *createSpaceLayoutReceiver();

void onCreateSpaceLayout(EditorContext *ctx);
void onDestroySpaceLayout(EditorContext *ctx);

void onUpdateSpaceLayout(EditorContext *ctx);
void onResizeSpaceLayout(EditorContext *ctx);

void onRenderSpaceLayout(EditorContext *ctx);
void onRenderPostProcessingSpaceLayout(EditorContext *ctx);

void onDrawUISpaceLayout(EditorContext *ctx);

#endif // SPACE_LAYOUT_H
