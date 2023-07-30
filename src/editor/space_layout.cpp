#include "space_layout.h"

#include "imgui/imgui.h"

EventReceiver *createSpaceLayoutReceiver() {
  EventReceiver *receiver = new EventReceiver();

  receiver->clearEvents();

  receiver->onCreate = onCreateSpaceLayout;
  receiver->onDestroy = onDestroySpaceLayout;

  receiver->onUpdate = onUpdateSpaceLayout;
  receiver->onResize = onResizeSpaceLayout;

  receiver->onRender = onRenderSpaceLayout;
  receiver->onRenderPostProcessing = onRenderPostProcessingSpaceLayout;

  receiver->onDrawUI = onDrawUISpaceLayout;

  return receiver;
}

void onCreateSpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = new SpaceLayoutData();
  ctx->space_layout_data = space_data;

  space_data->view3d_region = new View3dRegion();
  space_data->view3d_region->v3d_mode = V3D_OBJECT;
  space_data->view3d_region->createRenderTargets();
}

void onDestroySpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;

  space_data->view3d_region->destroyRenderTargets();
  delete space_data->view3d_region;

  delete space_data;
}

void onUpdateSpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;

  space_data->view3d_region->onUpdate(ctx, (void *)space_data);
}

void onResizeSpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;

  space_data->view3d_region->onResize(ctx);
}

void onRenderSpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;

  space_data->view3d_region->onRender(ctx, (void *)space_data);
}

void onRenderPostProcessingSpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;

  space_data->view3d_region->onRenderPostProcessing(ctx);
}

void onDrawUISpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;

  ImGui::Begin("Layout");

  space_data->view3d_region->onDrawUI(ctx);

  ImGui::End();
}