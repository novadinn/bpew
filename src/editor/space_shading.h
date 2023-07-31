#ifndef SPACE_SHADING_H
#define SPACE_SHADING_H

#include "../ecs/entity.h"
#include "../graphics/material.h"
#include "../graphics/material_manager.h"
#include "../nodes/node.h"
#include "event_receiver.h"

struct SpaceShadingData {
  char unused;
  int active_material_index = -1;

  void createNode(Material *mat, NodeType type);
  Material *activeMaterial();
};

EventReceiver *createSpaceShadingReceiver();

void onCreateSpaceShading(EditorContext *ctx);
void onDestroySpaceShading(EditorContext *ctx);

void onDrawUISpaceShading(EditorContext *ctx);
void onDrawUIEndSpaceShading(EditorContext *ctx);

NodePropertyType mixType(int value);

void drawNode(SpaceShadingData *ctx, Node &node);
void drawNodeInputEnd(SpaceShadingData *ctx, NodeInput &input);
bool drawNodeInputBegin(SpaceShadingData *ctx, NodeInput &input);
void drawNodeInputEnd();
void drawNodeOutputAttribute(SpaceShadingData *ctx, NodeOutput &output);

bool inputDrawable(SpaceShadingData *ctx, NodeInput &input);
void drawNodeInputAttributes(SpaceShadingData *ctx,
                             std::vector<NodeInput> &inputs);
void drawNodeOutputAttributes(SpaceShadingData *ctx,
                              std::vector<NodeOutput> &outputs);

void drawNodeRGB(SpaceShadingData *ctx, Node &node);
void drawNodeBevel(SpaceShadingData *ctx, Node &node);
void drawNodeMaterialOutput(SpaceShadingData *ctx, Node &node);
void drawNodeBrickTexture(SpaceShadingData *ctx, Node &node);
void drawNodeCheckerTexture(SpaceShadingData *ctx, Node &node);
void drawNodeImageTexture(SpaceShadingData *ctx, Node &node);
void drawNodeBrightnessContrast(SpaceShadingData *ctx, Node &node);
void drawNodeGamma(SpaceShadingData *ctx, Node &node);
void drawNodeInvert(SpaceShadingData *ctx, Node &node);
void drawNodeLightFalloff(SpaceShadingData *ctx, Node &node);
void drawNodeTextureCoordinate(SpaceShadingData *ctx, Node &node);
void drawNodePrincipledBSDF(SpaceShadingData *ctx, Node &node);
void drawNodeMix(SpaceShadingData *ctx, Node &node);

void drawNodeInputFloatDrag(NodeInput &input,
                            const char *title, float min = 0.0f,
                            float max = 0.0f);
void drawNodeInputIntSlider(NodeInput &input,
														const char *title, int min = 0,
														int max = 0);
void drawNodeInputFloatDrag3(NodeInput &input,
                             const char *title, float min = 0.0f,
                             float max = 0.0f);
void drawNodeInputColor3Picker(NodeInput &input,
                               const char *title, ImGuiColorEditFlags = 0);
void drawNodeInputColor3Edit(NodeInput &input,
                             const char *title, ImGuiColorEditFlags flags = 0);
void drawNodeInputSelect(NodeInput &input,
                         const char *title, const char **names, uint size);

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
