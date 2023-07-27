#include "space_layout.h"

#include "../core/input.h"
#include "../core/utils.h"
#include "../graphics/gizmos.h"
#include "../graphics/renderer.h"

#include "ImGuizmo/ImGuizmo.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

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

  FramebufferData data;
  data.formats = {GL_RGBA8, GL_RGBA32F, GL_DEPTH24_STENCIL8};
  data.width = 800;
  data.height = 800;
  space_data->framebuffer.create(data);

  data.formats = {GL_RGBA8};
  space_data->ping_pong_buffer.create(data, data);
}

void onDestroySpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;

  space_data->ping_pong_buffer.destroy();
  space_data->framebuffer.destroy();

  delete space_data;
}

void onUpdateSpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;

  if (Input::wasMouseButtonPressed(SDL_BUTTON_LEFT)) {
    if (space_data->viewport_hovered && !ImGuizmo::IsOver()) {
      ctx->active_entity = space_data->hovered_entity;

      /* miss */
      if (!ctx->active_entity) {
        ctx->selected_entities.clear();
      } else { /* hit */
        if (ctx->selected_entities.empty()) {
          ctx->selected_entities.push_back(ctx->active_entity);
        } else if (Input::wasKeyHeld(SDLK_LSHIFT)) {
          bool contains = ctx->entitySelected(ctx->active_entity);

          if (contains) {
            for (int i = 0; i < ctx->selected_entities.size(); ++i) {
              if (ctx->selected_entities[i] == ctx->active_entity) {
                /* if entity already presented in the list, remove it */
                ctx->selected_entities.erase(ctx->selected_entities.begin() +
                                             i);
                break;
              }
            }
          } else {
            /* entity is not in the list, just add it */
            ctx->selected_entities.push_back(ctx->active_entity);
          }
        } else {
          /* without shift we need to deselect all entities and push the new one
           */
          ctx->selected_entities.clear();
          ctx->selected_entities.push_back(ctx->active_entity);
        }
      }
    }
  }

  if (Input::wasKeyPressed(SDLK_q)) {
    space_data->gizmo_operation = -1;
  } else if (Input::wasKeyPressed(SDLK_t)) {
    space_data->gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
  } else if (Input::wasKeyPressed(SDLK_r)) {
    space_data->gizmo_operation = ImGuizmo::OPERATION::ROTATE;
  } else if (Input::wasKeyPressed(SDLK_s)) {
    space_data->gizmo_operation = ImGuizmo::OPERATION::SCALE;
  }

  if (Input::wasKeyHeld(SDLK_z)) {
    if (Input::wasKeyPressed(SDLK_1)) {
      space_data->draw_mode = DrawMode::WIREFRAME;
    } else if (Input::wasKeyPressed(SDLK_2)) {
      space_data->draw_mode = DrawMode::RENDERED;
    } else if (Input::wasKeyPressed(SDLK_3)) {
      space_data->draw_mode = DrawMode::SOLID;
    } else if (Input::wasKeyPressed(SDLK_4)) {
      space_data->draw_mode = DrawMode::MATERIAL_PREVIEW;
    }
  }

  if (Input::wasKeyPressed(SDLK_DELETE) && ctx->active_entity) {
    ctx->destroyEntity();
  }

  switch (space_data->draw_mode) {
  case DrawMode::RENDERED:
    ctx->scene->onUpdateRendered();
    break;
  case DrawMode::MATERIAL_PREVIEW:
    ctx->scene->onUpdateMaterialPreview();
    break;
  }
}

void onResizeSpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;

  ctx->editor_camera->setViewportSize(space_data->viewport_size.x,
                                      space_data->viewport_size.y);
  ctx->scene->onResize(space_data->viewport_size.x,
                       space_data->viewport_size.y);

  FramebufferData data = space_data->framebuffer.getFramebufferData();

  if (space_data->viewport_size.x > 0.0f &&
      space_data->viewport_size.y > 0.0f &&
      (data.width != space_data->viewport_size.x ||
       data.height != space_data->viewport_size.y)) {

    space_data->framebuffer.resize(space_data->viewport_size.x,
                                   space_data->viewport_size.y);
    space_data->ping_pong_buffer.resize(space_data->viewport_size.x,
                                        space_data->viewport_size.y);
  }
}

void onRenderSpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;
  RendererContext *renderer_context = ctx->renderer_context;

  space_data->framebuffer.bind();
  Renderer::clear();
  space_data->framebuffer.clearRGBA8ColorAttachment(1, glm::vec4(-1));

  glm::mat4 view = ctx->editor_camera->getViewMatrix();
  glm::mat4 projection = ctx->editor_camera->getProjectionMatrix();
  glm::vec3 view_pos = ctx->editor_camera->position;
  glm::vec3 direction = ctx->editor_camera->getForward();

  renderer_context->setCameraData(view, projection);
  renderer_context->setEditorLightData(view_pos, direction);

  switch (space_data->draw_mode) {
  case DrawMode::WIREFRAME: {
    ctx->scene->onDrawWireframe(renderer_context);
  } break;
  case DrawMode::RENDERED: {
    ctx->scene->onDrawRendered(renderer_context);
  } break;
  case DrawMode::SOLID: {
    ctx->scene->onDrawSolid(renderer_context);
  } break;
  case DrawMode::MATERIAL_PREVIEW: {
    ctx->scene->onDrawMaterialPreview(renderer_context);
  } break;
  }

  auto [mx, my] = ImGui::GetMousePos();
  mx -= space_data->viewport_bounds[0].x;
  my -= space_data->viewport_bounds[0].y;
  glm::vec2 viewport_size =
      space_data->viewport_bounds[1] - space_data->viewport_bounds[0];
  my = viewport_size.y - my;
  int mouse_x = (int)mx;
  int mouse_y = (int)my;

  if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < (int)viewport_size.x &&
      mouse_y < (int)viewport_size.y) {

    space_data->framebuffer.bindReadAttachment(1);

    glm::vec4 pixel_data =
        space_data->framebuffer.readRGBA8Pixel(mouse_x, mouse_y);
    int id = (int)pixel_data.x;
    if (id != -1) {
      Entity entity;
      entity.create((entt::entity)id, ctx->scene);
      space_data->hovered_entity = entity;
    } else {
      space_data->hovered_entity = {};
    }

    space_data->framebuffer.bindReadAttachment(0);
  }

  /* draw lines */
  float far = ctx->editor_camera->far;

  for (float x = view_pos.x - far; x < view_pos.x + far; x += 0.5f) {
    glm::vec3 start = glm::vec3((int)x, 0, (int)(view_pos.z - far));
    glm::vec3 end = glm::vec3((int)x, 0, (int)(view_pos.z + far));
    glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
    if ((int)x == 0) {
      color = glm::vec3(1, 0.4, 0.4);
    }

    Gizmos::drawLine(view, projection, start, end, color);
  }

  for (float z = view_pos.z - far; z < view_pos.z + far; z += 0.5f) {
    glm::vec3 start = glm::vec3((int)(view_pos.x - far), 0, (int)z);
    glm::vec3 end = glm::vec3((int)(view_pos.x + far), 0, (int)z);
    glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
    if ((int)z == 0) {
      color = glm::vec3(0.55, 0.8, 0.9);
    }

    Gizmos::drawLine(view, projection, start, end, color);
  }

  space_data->framebuffer.unbind();
}

void onRenderPostProcessingSpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;
  RendererContext *renderer_context = ctx->renderer_context;

  uint pong_id = space_data->framebuffer.getColorAttachmentID(0);
  bool do_postprocessing = false;

  for (auto entity : ctx->selected_entities) {
    if (entity.hasComponent<MeshComponent>()) {
      do_postprocessing = true;

      space_data->ping_pong_buffer.current->bind();

      renderer_context->setMeshOutlineData(
          pong_id, space_data->framebuffer.getColorAttachmentID(1),
          (uint32)entity, glm::vec3(0.0f, 0.0f, 1.0f), 0.5f);
      Renderer::applyMeshOutline(renderer_context);

      space_data->ping_pong_buffer.current->unbind();

      pong_id = space_data->ping_pong_buffer.current->getColorAttachmentID(0);
      space_data->ping_pong_buffer.swap();
    }
  }

  space_data->ping_pong_buffer.current->unbind();
  space_data->ping_pong_buffer.swap();

  /* since ping pong texture might be empty, we dont want to set color texture
   * as an empty one */
  uint target_color_texture =
      do_postprocessing
          ? space_data->ping_pong_buffer.current->getColorAttachmentID(0)
          : space_data->framebuffer.getColorAttachmentID(0);

  renderer_context->setFXAAData(space_data->viewport_size,
                                target_color_texture);

  space_data->ping_pong_buffer.swap();

  space_data->ping_pong_buffer.current->bind();
  Renderer::clear();
  Renderer::applyFXAA(renderer_context);

  space_data->ping_pong_buffer.current->unbind();
}

void onDrawUISpaceLayout(EditorContext *ctx) {
  SpaceLayoutData *space_data = ctx->space_layout_data;

  ImGui::Begin("Layout");

  space_data->viewport_size = Utils::getAvailableViewportSize();
  space_data->viewport_bounds[0] = Utils::getAvailableViewportBoundsMin();
  space_data->viewport_bounds[1] = Utils::getAvailableViewportBoundsMax();
  space_data->viewport_hovered = Utils::isViewportHovered();

  ImGui::Image(
      reinterpret_cast<void *>(
          space_data->ping_pong_buffer.current->getColorAttachmentID(0)),
      ImVec2{space_data->viewport_size.x, space_data->viewport_size.y},
      ImVec2{0, 1}, ImVec2{1, 0});

  glm::mat4 view = ctx->editor_camera->getViewMatrix();
  glm::mat4 projection = ctx->editor_camera->getProjectionMatrix();

  /* Draw settings */
  if (ImGui::Begin("Layout Settings")) {
    ImGui::Text("Render Mode");
    /* TODO: use icons instead of words */
    if (ImGui::Button("Wireframe")) {
      space_data->draw_mode = DrawMode::WIREFRAME;
    }
    ImGui::SameLine();
    if (ImGui::Button("Rendered")) {
      space_data->draw_mode = DrawMode::RENDERED;
    }
    ImGui::SameLine();
    if (ImGui::Button("Solid")) {
      space_data->draw_mode = DrawMode::SOLID;
    }
    ImGui::SameLine();
    if (ImGui::Button("Material")) {
      space_data->draw_mode = DrawMode::MATERIAL_PREVIEW;
    }

    ImGui::Text("Gizmos Type");
    if (ImGui::Button("Translate")) {
      space_data->gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
    }
    ImGui::SameLine();
    if (ImGui::Button("Rotate")) {
      space_data->gizmo_operation = ImGuizmo::OPERATION::ROTATE;
    }
    ImGui::SameLine();
    if (ImGui::Button("Scale")) {
      space_data->gizmo_operation = ImGuizmo::OPERATION::SCALE;
    }
    ImGui::SameLine();
    if (ImGui::Button("None")) {
      space_data->gizmo_operation = -1;
    }
    ImGui::SameLine();

    ImGui::End();
  }

  /* draw gizmos */
  if (ctx->selected_entities.size() > 0 && space_data->gizmo_operation != -1) {
    /* take this as a sample transform */
    glm::mat4 avg_model = glm::mat4(0.0f);

    /* average model matrix */
    for (int i = 0; i < ctx->selected_entities.size(); ++i) {
      ASSERT(ctx->selected_entities[i].hasComponent<TransformComponent>());
      TransformComponent &transform =
          ctx->selected_entities[i].getComponent<TransformComponent>();

      avg_model += transform.getModelMatrix();
    }
    avg_model /= ctx->selected_entities.size();

    float *model_ptr = glm::value_ptr(avg_model);
    float prev_position[3];
    float prev_rotation[3];
    float prev_scale[3];

    /* store previous averaged results */
    ImGuizmo::DecomposeMatrixToComponents(model_ptr, prev_position,
                                          prev_rotation, prev_scale);

    bool snap = Input::wasKeyHeld(SDLK_LCTRL);

    Gizmos::drawManupilations((ImGuizmo::OPERATION)space_data->gizmo_operation,
                              glm::value_ptr(view), glm::value_ptr(projection),
                              glm::value_ptr(avg_model), snap);

    if (ImGuizmo::IsUsing()) {
      float scale_result[3];
      float rotation_result[3];
      float translation_result[3];
      ImGuizmo::DecomposeMatrixToComponents(model_ptr, translation_result,
                                            rotation_result, scale_result);

      for (int i = 0; i < ctx->selected_entities.size(); ++i) {
        TransformComponent &target =
            ctx->selected_entities[i].getComponent<TransformComponent>();

        target.position +=
            glm::vec3(translation_result[0], translation_result[1],
                      translation_result[2]) -
            glm::vec3(prev_position[0], prev_position[1], prev_position[2]);
        target.rotation +=
            glm::vec3(rotation_result[0], rotation_result[1],
                      rotation_result[2]) -
            glm::vec3(prev_rotation[0], prev_rotation[1], prev_rotation[2]);
        target.scale +=
            glm::vec3(scale_result[0], scale_result[1], scale_result[2]) -
            glm::vec3(prev_scale[0], prev_scale[1], prev_scale[2]);
      }
    }
  }

  ImGui::End();
}