#include "space_modeling.h"

#include "../core/input.h"
#include "../core/log.h"
#include "../core/utils.h"
#include "../graphics/gizmos.h"
#include "../graphics/renderer.h"

#include "ImGuizmo/ImGuizmo.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

EventReceiver *createSpaceModelingReceiver() {
  EventReceiver *receiver = new EventReceiver();

  receiver->clearEvents();

  receiver->onCreate = onCreateSpaceModeling;
  receiver->onDestroy = onDestroySpaceModeling;

  receiver->onUpdate = onUpdateSpaceModeling;
  receiver->onResize = onResizeSpaceModeling;

  receiver->onRender = onRenderSpaceModeling;
  receiver->onRenderPostProcessing = onRenderPostProcessingSpaceModeling;

  receiver->onDrawUI = onDrawUISpaceModeling;

  return receiver;
}

void onCreateSpaceModeling(EditorContext *ctx) {
  SpaceModelingData *space_data = new SpaceModelingData();
  ctx->space_modeling_data = space_data;

  space_data->view3d_region = new View3dRegion();
  space_data->view3d_region->v3d_mode = V3D_EDIT;
  space_data->view3d_region->createRenderTargets();
}

void onDestroySpaceModeling(EditorContext *ctx) {
  SpaceModelingData *space_data = ctx->space_modeling_data;

  space_data->view3d_region->destroyRenderTargets();
  delete space_data->view3d_region;

  delete space_data;
}

void onUpdateSpaceModeling(EditorContext *ctx) {
  SpaceModelingData *space_data = ctx->space_modeling_data;

  space_data->view3d_region->onUpdate(ctx, (void *)space_data);
}

void onResizeSpaceModeling(EditorContext *ctx) {
  SpaceModelingData *space_data = ctx->space_modeling_data;

  space_data->view3d_region->onResize(ctx);
}

void onRenderSpaceModeling(EditorContext *ctx) {
  SpaceModelingData *space_data = ctx->space_modeling_data;

  space_data->view3d_region->onRender(ctx, (void *)space_data);
}

void onRenderPostProcessingSpaceModeling(EditorContext *ctx) {
  SpaceModelingData *space_data = ctx->space_modeling_data;

  space_data->view3d_region->onRenderPostProcessing(ctx);
}

void onDrawUISpaceModeling(EditorContext *ctx) {
  SpaceModelingData *space_data = ctx->space_modeling_data;

  ImGui::Begin("Modeling");

  space_data->view3d_region->onDrawUI(ctx);

  ImGui::End();
}
