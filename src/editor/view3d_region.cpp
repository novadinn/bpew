#include "view3d_region.h"

#include "../core/input.h"
#include "../core/utils.h"
#include "../graphics/gizmos.h"
#include "../graphics/renderer.h"
#include "space_layout.h"

#include "ImGuizmo/ImGuizmo.h"
#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

void View3dRegion::createRenderTargets() {
  FramebufferData data;
  data.formats = {GL_RGBA8, GL_RGBA32F, GL_DEPTH24_STENCIL8};
  data.width = 800;
  data.height = 800;
  framebuffer.create(data);

  data.formats = {GL_RGBA8};
  ping_pong_buffer.create(data, data);
}

void View3dRegion::destroyRenderTargets() {
  ping_pong_buffer.destroy();
  framebuffer.destroy();
}

void View3dRegion::onUpdate(EditorContext *ctx, void *space) {
  if (Input::wasMouseButtonPressed(SDL_BUTTON_LEFT)) {
    if (viewport_hovered && !ImGuizmo::IsOver()) {
      switch (v3d_mode) {
      case V3D_OBJECT: {
        SpaceLayoutData *space_data = (SpaceLayoutData *)space;
        ASSERT(space_data);

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
            /* without shift we need to deselect all entities and push the new
             * one
             */
            ctx->selected_entities.clear();
            ctx->selected_entities.push_back(ctx->active_entity);
          }
        }
      } break;
      case V3D_EDIT: {
        uint32 entity_id = 0;
        int vertex_id = -1;
        glm::vec3 direction =
            ctx->editor_camera->screenToWorldDirection(mouse_position);
        ctx->scene->searchIntersectedVertices(
            &entity_id, &vertex_id, ctx->editor_camera->position, direction);

        if (entity_id != 0 && vertex_id != -1) {
          Entity entity;
          entity.create((entt::entity)entity_id, ctx->scene);
          ctx->active_entity = entity;
          ctx->active_vertex = std::pair(entity, vertex_id);
        } else {
          ctx->active_entity = {};
          ctx->active_vertex = std::pair(Entity{}, -1);
        }

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
            /* without shift we need to deselect all entities and push the new
             * one
             */
            ctx->selected_entities.clear();
            ctx->selected_entities.push_back(ctx->active_entity);
          }
        }

        /* miss */
        if (!ctx->active_vertex.first && ctx->active_vertex.second == -1) {
          ctx->selected_vertices.clear();
        } else { /* hit */
          if (ctx->selected_vertices.empty()) {
            ctx->selected_vertices.push_back(ctx->active_vertex);
          } else if (Input::wasKeyHeld(SDLK_LSHIFT)) {
            bool contains = ctx->vertexSelected(ctx->active_vertex);

            if (contains) {
              for (int i = 0; i < ctx->selected_vertices.size(); ++i) {
                if (ctx->selected_vertices[i] == ctx->active_vertex) {
                  /* if entity already presented in the list, remove it */
                  ctx->selected_vertices.erase(ctx->selected_vertices.begin() +
                                               i);
                  break;
                }
              }
            } else {
              /* entity is not in the list, just add it */
              ctx->selected_vertices.push_back(ctx->active_vertex);
            }
          } else {
            /* without shift we need to deselect all entities and push the new
             * one
             */
            ctx->selected_vertices.clear();
            ctx->selected_vertices.push_back(ctx->active_vertex);
          }
        }
      } break;
      };
    }
  }

  if (Input::wasKeyPressed(SDLK_q)) {
    gizmo_operation = -1;
  } else if (Input::wasKeyPressed(SDLK_t)) {
    gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
  } else if (Input::wasKeyPressed(SDLK_r)) {
    gizmo_operation = ImGuizmo::OPERATION::ROTATE;
  } else if (Input::wasKeyPressed(SDLK_s)) {
    gizmo_operation = ImGuizmo::OPERATION::SCALE;
  }

  if (Input::wasKeyHeld(SDLK_z)) {
    if (Input::wasKeyPressed(SDLK_1)) {
      draw_mode = DM_WIREFRAME;
    } else if (Input::wasKeyPressed(SDLK_2)) {
      draw_mode = DM_RENDERED;
    } else if (Input::wasKeyPressed(SDLK_3)) {
      draw_mode = DM_SOLID;
    } else if (Input::wasKeyPressed(SDLK_4)) {
      draw_mode = DM_MATERIAL_PREVIEW;
    }
  }

  switch (v3d_mode) {
  case V3D_OBJECT: {
    if (Input::wasKeyPressed(SDLK_DELETE) && ctx->active_entity) {
      ctx->destroyEntity();
    }
  } break;
  }

  /* TODO: this should not be in here, since this could be directly set from
   * rederer */
  switch (draw_mode) {
  case DM_RENDERED:
    ctx->scene->onUpdateRendered();
    break;
  case DM_MATERIAL_PREVIEW:
    ctx->scene->onUpdateMaterialPreview();
    break;
  }
}

void View3dRegion::onResize(EditorContext *ctx) {
  ctx->editor_camera->setViewportSize(viewport_size.x, viewport_size.y);
  ctx->scene->onResize(viewport_size.x, viewport_size.y);

  FramebufferData data = framebuffer.getFramebufferData();

  if (viewport_size.x > 0.0f && viewport_size.y > 0.0f &&
      (data.width != viewport_size.x || data.height != viewport_size.y)) {

    framebuffer.resize(viewport_size.x, viewport_size.y);
    ping_pong_buffer.resize(viewport_size.x, viewport_size.y);
  }
}

void View3dRegion::onRender(EditorContext *ctx, void *space) {
  RendererContext *renderer_context = ctx->renderer_context;

  framebuffer.bind();
  Renderer::clear();
  framebuffer.clearRGBA8ColorAttachment(1, glm::vec4(-1));

  glm::mat4 view = ctx->editor_camera->getViewMatrix();
  glm::mat4 projection = ctx->editor_camera->getProjectionMatrix();
  glm::vec3 view_pos = ctx->editor_camera->position;
  glm::vec3 direction = ctx->editor_camera->getForward();

  renderer_context->setCameraData(view, projection);
  renderer_context->setEditorLightData(view_pos, direction);

  switch (draw_mode) {
  case DM_WIREFRAME: {
    ctx->scene->onDrawWireframe(renderer_context);
  } break;
  case DM_RENDERED: {
    ctx->scene->onDrawRendered(renderer_context);
  } break;
  case DM_SOLID: {
    ctx->scene->onDrawSolid(renderer_context);
  } break;
  case DM_MATERIAL_PREVIEW: {
    ctx->scene->onDrawMaterialPreview(renderer_context);
  } break;
  }

  auto [mx, my] = ImGui::GetMousePos();
  mx -= viewport_bounds[0].x;
  my -= viewport_bounds[0].y;
  glm::vec2 viewport_size = viewport_bounds[1] - viewport_bounds[0];
  my = viewport_size.y - my;
  int mouse_x = (int)mx;
  int mouse_y = (int)my;
  mouse_position = {mouse_x, mouse_y};

  switch (v3d_mode) {
  case V3D_OBJECT: {
    if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < (int)viewport_size.x &&
        mouse_y < (int)viewport_size.y) {
      SpaceLayoutData *space_data = (SpaceLayoutData *)space;
      ASSERT(space_data);

      framebuffer.bindReadAttachment(1);

      glm::vec4 pixel_data = framebuffer.readRGBA8Pixel(mouse_x, mouse_y);
      int id = (int)pixel_data.x;
      if (id != -1) {
        Entity entity;
        entity.create((entt::entity)id, ctx->scene);
        space_data->hovered_entity = entity;
      } else {
        space_data->hovered_entity = {};
      }

      framebuffer.bindReadAttachment(0);
    }
  } break;
  case V3D_EDIT: {
    /* draw vertices */
    framebuffer.clearRGBA8ColorAttachment(1, glm::vec4(-1));

    renderer_context->setCameraData(view, projection);
    renderer_context->setMeshVerticesData(glm::vec3(0.0));

    ctx->scene->onDrawMeshVertices(renderer_context);
  } break;
  }

  /* draw lines */
  /* TODO: some linese dissapears when rotating the scene */
  const float far = ctx->editor_camera->far;
  const glm::vec3 camera_position = ctx->editor_camera->position;

  for (float x = view_pos.x - far; x < view_pos.x + far; x += 0.5f) {
    glm::vec3 start = glm::vec3((int)x, 0, (int)(view_pos.z - far));
    glm::vec3 end = glm::vec3((int)x, 0, (int)(view_pos.z + far));
    glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
    if ((int)x == 0) {
      color = glm::vec3(1, 0.4, 0.4);
    }

    Gizmos::drawLine(view, projection, start, end, color, camera_position, far);
  }

  for (float z = view_pos.z - far; z < view_pos.z + far; z += 0.5f) {
    glm::vec3 start = glm::vec3((int)(view_pos.x - far), 0, (int)z);
    glm::vec3 end = glm::vec3((int)(view_pos.x + far), 0, (int)z);
    glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
    if ((int)z == 0) {
      color = glm::vec3(0.55, 0.8, 0.9);
    }

    Gizmos::drawLine(view, projection, start, end, color, camera_position, far);
  }

  framebuffer.unbind();
}

void View3dRegion::onRenderPostProcessing(EditorContext *ctx) {
  RendererContext *renderer_context = ctx->renderer_context;

  uint pong_id = framebuffer.getColorAttachmentID(0);

  switch (v3d_mode) {
  case V3D_OBJECT: {
    for (auto entity : ctx->selected_entities) {
      if (entity.hasComponent<MeshComponent>()) {
        ping_pong_buffer.current->bind();

        renderer_context->setMeshOutlineData(
            pong_id, framebuffer.getColorAttachmentID(1), (uint32)entity,
            glm::vec3(0.0f, 0.0f, 1.0f), 0.5f);
        Renderer::applyMeshOutline(renderer_context);

        ping_pong_buffer.current->unbind();

        pong_id = ping_pong_buffer.current->getColorAttachmentID(0);
        ping_pong_buffer.swap();
      }
    }
  } break;
  case V3D_EDIT: {
    for (int i = 0; i < ctx->selected_vertices.size(); ++i) {
      if (ctx->selected_vertices[i].first.hasComponent<MeshComponent>()) {
        ping_pong_buffer.current->bind();

        renderer_context->setVertexOutlineData(
            pong_id, framebuffer.getColorAttachmentID(1),
            (uint32)ctx->selected_vertices[i].first,
            ctx->selected_vertices[i].second, glm::vec3(0.0f, 0.0f, 1.0f),
            1.0f);
        Renderer::applyVertexOutline(renderer_context);

        ping_pong_buffer.current->unbind();

        pong_id = ping_pong_buffer.current->getColorAttachmentID(0);
        ping_pong_buffer.swap();
      }
    }

  } break;
  };

  renderer_context->setFXAAData(viewport_size, pong_id);

  ping_pong_buffer.current->bind();
  Renderer::applyFXAA(renderer_context);

  ping_pong_buffer.current->unbind();
}

void View3dRegion::onDrawUI(EditorContext *ctx) {
  viewport_size = Utils::getAvailableViewportSize();
  viewport_bounds[0] = Utils::getAvailableViewportBoundsMin();
  viewport_bounds[1] = Utils::getAvailableViewportBoundsMax();
  viewport_hovered = Utils::isViewportHovered();

  ImGui::Image(reinterpret_cast<void *>(
                   ping_pong_buffer.current->getColorAttachmentID(0)),
               ImVec2{viewport_size.x, viewport_size.y}, ImVec2{0, 1},
               ImVec2{1, 0});

  /* Draw settings */
  if (ImGui::Begin("View3D Settings")) {
    ImGui::Text("Render Mode");
    /* TODO: use icons instead of words */
    if (ImGui::Button("Wireframe")) {
      draw_mode = DM_WIREFRAME;
    }
    ImGui::SameLine();
    if (ImGui::Button("Rendered")) {
      draw_mode = DM_RENDERED;
    }
    ImGui::SameLine();
    if (ImGui::Button("Solid")) {
      draw_mode = DM_SOLID;
    }
    ImGui::SameLine();
    if (ImGui::Button("Material")) {
      draw_mode = DM_MATERIAL_PREVIEW;
    }

    ImGui::Text("Gizmos Type");
    if (ImGui::Button("Translate")) {
      gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
    }
    ImGui::SameLine();
    if (ImGui::Button("Rotate")) {
      gizmo_operation = ImGuizmo::OPERATION::ROTATE;
    }
    ImGui::SameLine();
    if (ImGui::Button("Scale")) {
      gizmo_operation = ImGuizmo::OPERATION::SCALE;
    }
    ImGui::SameLine();
    if (ImGui::Button("None")) {
      gizmo_operation = -1;
    }
    ImGui::SameLine();

    ImGui::End();
  }

  glm::mat4 view = ctx->editor_camera->getViewMatrix();
  glm::mat4 projection = ctx->editor_camera->getProjectionMatrix();

  /* draw gizmos */
  switch (v3d_mode) {
  case V3D_OBJECT: {
    if (ctx->selected_entities.size() > 0 && gizmo_operation != -1) {
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

      Gizmos::drawManupilations(
          (ImGuizmo::OPERATION)gizmo_operation, glm::value_ptr(view),
          glm::value_ptr(projection), glm::value_ptr(avg_model), snap);

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
  } break;
  case V3D_EDIT: {
    if (ctx->selected_vertices.size() > 0 && gizmo_operation != -1) {
      glm::mat4 avg_model = glm::mat4(0.0f);
      glm::vec4 avg_position = glm::vec4(0.0f);

      for (int i = 0; i < ctx->selected_vertices.size(); ++i) {
        ASSERT(
            ctx->selected_vertices[i].first.hasComponent<TransformComponent>());
        TransformComponent &transform =
            ctx->selected_vertices[i].first.getComponent<TransformComponent>();

        avg_model += transform.getModelMatrix();

        ASSERT(ctx->selected_vertices[i].first.hasComponent<MeshComponent>());
        MeshComponent &mesh_component =
            ctx->selected_vertices[i].first.getComponent<MeshComponent>();

        for (int j = 0; j < mesh_component.meshes.size(); ++j) {
          Mesh &mesh = mesh_component.meshes[j];
          int index =
              ctx->selected_vertices[i].second * mesh.totalAttributesCount();

          /* TODO: not sure if this will work in case there is more that 1 mesh
           */
          ASSERT(index < mesh.vertices.size());
          float *vertex = &mesh.vertices[index];

          glm::vec3 vertex_position =
              glm::vec3(vertex[0], vertex[1], vertex[2]);
          glm::vec4 world_space_position =
              transform.getModelMatrix() * glm::vec4(vertex_position, 1.0);

          avg_position += world_space_position;
        }
      }

      avg_model /= ctx->selected_vertices.size();
      avg_position /= ctx->selected_vertices.size();
      /* override translation */
      avg_model[3] = glm::vec4(glm::vec3(avg_position), 1.0f);

      float *model_ptr = glm::value_ptr(avg_model);
      float prev_position[3];
      float prev_rotation[3];
      float prev_scale[3];

      /* store previous averaged results */
      ImGuizmo::DecomposeMatrixToComponents(model_ptr, prev_position,
                                            prev_rotation, prev_scale);

      bool snap = Input::wasKeyHeld(SDLK_LCTRL);

      /* TODO: gizmos are a bit wrong (they are offseted in the y direction
       * for some reason)
       */
      Gizmos::drawManupilations(
          (ImGuizmo::OPERATION)gizmo_operation, glm::value_ptr(view),
          glm::value_ptr(projection), glm::value_ptr(avg_model), snap);

      if (ImGuizmo::IsUsing()) {
        float scale_result[3];
        float rotation_result[3];
        float translation_result[3];
        ImGuizmo::DecomposeMatrixToComponents(model_ptr, translation_result,
                                              rotation_result, scale_result);

        for (int i = 0; i < ctx->selected_vertices.size(); ++i) {
          MeshComponent &mesh_component =
              ctx->selected_vertices[i].first.getComponent<MeshComponent>();

          for (int j = 0; j < mesh_component.meshes.size(); ++j) {
            Mesh &mesh = mesh_component.meshes[j];
            int index =
                ctx->selected_vertices[i].second * mesh.totalAttributesCount();

            /* TODO: not sure if this will work in case there is more that 1
             * mesh
             */
            float *vertex = &mesh.vertices[index];

            vertex[0] += translation_result[0] - prev_position[0];
            vertex[1] += translation_result[1] - prev_position[1];
            vertex[2] += translation_result[2] - prev_position[2];

            /* TODO: those operations should change vertex normals
               transform.rotation = {rotation_result[0], rotation_result[1],
               rotation_result[2]}; transform.scale = {scale_result[0],
               scale_result[1], scale_result[2]}; */

            /* TODO: find a better way to update vertices data,
               since we dont need to recreate it every time */
            mesh.destroy();
            mesh.generateVertexArray();

            break;
          }
        }
      }
    }
  } break;
  };
}