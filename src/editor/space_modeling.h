#ifndef SPACE_MODELING_H
#define SPACE_MODELING_H

#include "event_receiver.h"
#include "view3d_region.h"

struct SpaceModelingData {
  View3dRegion *view3d_region;
};

EventReceiver *createSpaceModelingReceiver();

void onCreateSpaceModeling(EditorContext *ctx);
void onDestroySpaceModeling(EditorContext *ctx);

void onUpdateSpaceModeling(EditorContext *ctx);
void onResizeSpaceModeling(EditorContext *ctx);

void onRenderSpaceModeling(EditorContext *ctx);
void onRenderPostProcessingSpaceModeling(EditorContext *ctx);

void onDrawUISpaceModeling(EditorContext *ctx);

#endif // SPACE_MODELING_H
