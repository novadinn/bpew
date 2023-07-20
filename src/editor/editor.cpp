#include "editor.h"

#include "../core/input.h"
#include "../core/time.h"
#include "../core/utils.h"
#include "../ecs/components/camera_component.h"
#include "../ecs/components/light_component.h"
#include "../ecs/components/mesh_component.h"
#include "../ecs/components/tag_component.h"
#include "../ecs/components/transform_component.h"
#include "../ecs/scene_serializer.h"
#include "../graphics/gizmos.h"
#include "../graphics/material.h"
#include "../graphics/renderer.h"
#include "../graphics/shaders/shader_container.h"
#include "editor_camera.h"

#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "ImGuizmo/ImGuizmo.h"
#include "imgui/imgui.h"
#include "imnodes/imnodes.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

void Editor::create() {
  ctx = new EditorContext();

  receivers.push_back(createSpaceLayoutReceiver());
  receivers.push_back(createSpaceModelingReceiver());
  receivers.push_back(createSpaceShadingReceiver());
  active_receiver = receivers[0]; // Set space layout as active

  ctx->renderer_context = new RendererContext();

  for (EventReceiver *recv : receivers) {
    recv->onCreate(ctx);
  }

  float near = 0.1f;
  float far = 50.0f;

  ctx->scene = new Scene();

  ctx->editor_camera = new EditorCamera();
  ctx->editor_camera->create(45, 1.778f, near, far);

  Entity camera_entity = ctx->scene->createEntity("Camera");
  auto &camera_component = camera_entity.addComponent<CameraComponent>();

  Entity object = ctx->scene->createEntity("Monkey");

  auto &mesh = object.addComponent<MeshComponent>();
  mesh.loadFromPath("datafiles/primitives/cube.obj");

  Entity dir_light = ctx->scene->createEntity("DirectionalLight");
  auto &light_dir = dir_light.addComponent<LightComponent>();
  light_dir.type = LightComponent::LightType::DIRECTIONAL;
  auto &tr1 = dir_light.getComponent<TransformComponent>();
  tr1.rotation.x = -100;

  auto &tr2 = camera_entity.getComponent<TransformComponent>();
  tr2.position = glm::vec3(0, 0, 3);
}

void Editor::destroy() {
  for (EventReceiver *recv : receivers) {
    recv->onDestroy(ctx);
    delete recv;
  }

  delete ctx->renderer_context;
  delete ctx->scene;
  delete ctx;
}

void Editor::onUpdate() {
  float delta_time = Time::deltaTime();
  glm::ivec2 current_mouse;
  Input::getMousePosition(&current_mouse.x, &current_mouse.y);
  glm::vec2 mouse_delta = current_mouse - previous_mouse;
  mouse_delta *= delta_time;
  previous_mouse = current_mouse;

  glm::ivec2 wheel_movement;
  Input::getWheelMovement(&wheel_movement.x, &wheel_movement.y);

  if (Input::wasMouseButtonHeld(SDL_BUTTON_MIDDLE)) {
    if (Input::wasKeyHeld(SDLK_LSHIFT)) {
      if (!ctx->active_camera) {
        ctx->editor_camera->pan(mouse_delta);
      }
    } else {
      if (!ctx->active_camera) {
        ctx->editor_camera->rotate(mouse_delta);
      }
    }
  }
  if (Input::wasWheelMoved()) {
    if (ctx->active_camera) {
      auto &camera_component =
          ctx->active_camera.getComponent<CameraComponent>();
      auto &transform_component =
          ctx->active_camera.getComponent<TransformComponent>();
      camera_component.zoom(delta_time * wheel_movement.y,
                            transform_component.rotation);
    } else {
      ctx->editor_camera->zoom(delta_time * wheel_movement.y);
    }
  }
  if (Input::wasKeyPressed(SDLK_ESCAPE)) {
    if (ctx->active_camera) {
      ctx->active_camera = {};
    }
  }

  ASSERT(active_receiver->onUpdate != NULL);
  active_receiver->onUpdate(ctx);
}

void Editor::onDraw() {
  ASSERT(active_receiver->onResize != NULL);
  active_receiver->onResize(ctx);

  ASSERT(active_receiver->onRenderBegin != NULL);
  active_receiver->onRenderBegin(ctx);
  ASSERT(active_receiver->onRender != NULL);
  active_receiver->onRender(ctx);

  showLines();

  ASSERT(active_receiver->onRenderPostProcessing != NULL);
  active_receiver->onRenderPostProcessing(ctx);
  ASSERT(active_receiver->onRenderEnd != NULL);
  active_receiver->onRenderEnd(ctx);

  static bool dockspace_open = true;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags |=
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;

  // NOTE: dockspace begin
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);

  ImGui::PopStyleVar();

  ImGui::PopStyleVar(2);

  ImGuiIO &io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  } else {
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
    ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your "
                "code, or ");
    ImGui::SameLine(0.0f, 0.0f);
    if (ImGui::SmallButton("click here"))
      io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  }

  static bool show_demo = false;
  if (show_demo)
    ImGui::ShowDemoWindow(&show_demo);

  showMenuBar();
  showHierarchyPanel();
  showInspectorPanel();
  showCameraPanel();

  ASSERT(active_receiver->onDrawUIBegin != NULL);
  active_receiver->onDrawUIBegin(ctx);
  ASSERT(active_receiver->onDrawUI != NULL);
  active_receiver->onDrawUI(ctx);
  ASSERT(active_receiver->onDrawUIEnd != NULL);
  active_receiver->onDrawUIEnd(ctx);

  // NOTE: dockspace end
  ImGui::End();
}

void Editor::showMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("New")) {
      }
      if (ImGui::MenuItem("Open")) {
        ImGuiFileDialog::Instance()->OpenDialog("LoadSceneDlgKey",
                                                "Choose File", ".bpew", ".");
      }

      ImGui::Separator();
      if (ImGui::MenuItem("Save")) {
        ImGuiFileDialog::Instance()->OpenDialog("SaveSceneDlgKey",
                                                "Choose File", ".bpew", ".");
      }

      ImGui::Separator();
      if (ImGui::MenuItem("Import")) {
      }
      if (ImGui::MenuItem("Export")) {
      }

      ImGui::Separator();
      if (ImGui::MenuItem("Quit")) {
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("Undo", "CTRL+Z")) {
      }
      if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
      }
      if (ImGui::MenuItem("Repeat Last", "SHIFT+R", false, false)) {
      }

      ImGui::Separator();
      if (ImGui::MenuItem("Cut", "CTRL+X")) {
      }
      if (ImGui::MenuItem("Copy", "CTRL+C")) {
      }
      if (ImGui::MenuItem("Paste", "CTRL+V")) {
      }

      ImGui::Separator();
      if (ImGui::BeginMenu("Preferences")) {
        // TODO: open another window
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Window")) {
      if (ImGui::MenuItem("New Window")) {
      }
      if (ImGui::MenuItem("Toggle Window Fullscreen")) {
      }

      ImGui::Separator();
      if (ImGui::MenuItem("Next Workspace")) {
      }
      if (ImGui::MenuItem("Previous Workspace")) {
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginTabBar("EditorTabBar")) {
      if (ImGui::BeginTabItem("Layout")) {
        ASSERT(receivers.size() > 0);
        active_receiver = receivers[0];
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Modeling")) {
        ASSERT(receivers.size() > 1);
        active_receiver = receivers[1];
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Shading")) {
        ASSERT(receivers.size() > 2);
        active_receiver = receivers[2];
        ImGui::EndTabItem();
      }

      ImGui::EndTabBar();
    }

    ImGui::EndMainMenuBar();
  }

  if (ImGuiFileDialog::Instance()->Display("LoadSceneDlgKey")) {
    if (ImGuiFileDialog::Instance()->IsOk()) {
      std::string fp = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string name = ImGuiFileDialog::Instance()->GetCurrentFileName();

      /* TODO: ensure we dont want to save the current scene */

      /* TODO: just delete entities, we dont need to reallocate the scene */
      delete ctx->scene;
      ctx->scene = new Scene();
      ctx->selected_entity = {};
      ctx->selected_vertex = -1;
      /* TODO: since we are restoring context, we need to store the active
       * camera elsewhere (in scene, for example */
      ctx->active_camera = {};
      ctx->space_layout_data->hovered_entity = {};

      SceneSerializer::deserialize(ctx->scene, fp);
    }

    ImGuiFileDialog::Instance()->Close();
  }

  if (ImGuiFileDialog::Instance()->Display("SaveSceneDlgKey")) {
    if (ImGuiFileDialog::Instance()->IsOk()) {
      std::string fp = ImGuiFileDialog::Instance()->GetFilePathName();

      SceneSerializer::serialize(ctx->scene, fp);
    }

    ImGuiFileDialog::Instance()->Close();
  }

  if (ImGuiFileDialog::Instance()->Display("LoadMeshDlgKey")) {
    if (ImGuiFileDialog::Instance()->IsOk()) {
      std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string name = ImGuiFileDialog::Instance()->GetCurrentFileName();

      if (ctx->selected_entity) {
        if (ctx->selected_entity.hasComponent<MeshComponent>()) {
          MeshComponent &mesh =
              ctx->selected_entity.getComponent<MeshComponent>();
          mesh.loadFromPath(path.c_str());
        }
      }
    }

    ImGuiFileDialog::Instance()->Close();
  }
}

void Editor::showHierarchyPanel() {
  ImGui::Begin("Hierarchy");
  if (ImGui::CollapsingHeader("Scene")) {
    entt::registry &registry = ctx->scene->getEntityRegistry();
    auto view = registry.view<TagComponent>();
    for (auto entity : view) {
      auto &tag = view.get<TagComponent>(entity);
      if (ImGui::Selectable(tag.tag.c_str(), ctx->selected_entity == entity))
        ctx->selected_entity.create(entity, ctx->scene);
    }
  }

  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::MenuItem("Create Entity")) {
      ctx->scene->createEntity();
    }

    if (ctx->selected_entity) {
      if (ImGui::MenuItem("Destoy Entity")) {
        ctx->scene->destroyEntity(ctx->selected_entity);
        ctx->selected_entity = {};
      }
    }

    ImGui::EndPopup();
  }

  ImGui::End();
}

void Editor::showInspectorPanel() {
  ImGui::Begin("Inspector");

  if (ctx->selected_entity) {
    /* those 2 components always exists, dont need to check for them */
    ASSERT(ctx->selected_entity.hasComponent<TagComponent>());
    ImGui::Text("Tag");
    TagComponent &tag = ctx->selected_entity.getComponent<TagComponent>();
    char buff[256];
    memset(buff, 0, sizeof(buff));
    strncpy(buff, tag.tag.c_str(), sizeof(buff));
    if (ImGui::InputText("##Tag", buff, sizeof(buff))) {
      tag.tag = std::string(buff);
    }

    ASSERT(ctx->selected_entity.hasComponent<TransformComponent>());
    if (ImGui::CollapsingHeader("Transform")) {
      TransformComponent &transform =
          ctx->selected_entity.getComponent<TransformComponent>();

      float p[3] = {transform.position.x, transform.position.y,
                    transform.position.z};
      float s[3] = {transform.scale.x, transform.scale.y, transform.scale.z};
      float r[3] = {transform.rotation.x, transform.rotation.y,
                    transform.rotation.z};

      if (ImGui::DragFloat3("Position", p, 0.1f, 0.0f, 0.0f, "%.2f"))
        transform.position = glm::vec3(p[0], p[1], p[2]);
      if (ImGui::DragFloat3("Scale", s, 0.1f, 0.0f, 0.0f, "%.2f"))
        transform.scale = glm::vec3(s[0], s[1], s[2]);
      if (ImGui::DragFloat3("Rotation", r, 0.1f, 0.0f, 0.0f, "%.2f"))
        transform.rotation = glm::vec3(r[0], r[1], r[2]);
    }

    if (ctx->selected_entity.hasComponent<CameraComponent>()) {
      if (ImGui::CollapsingHeader("Camera")) {
        if (ImGui::Button("Select as active")) {
          ctx->active_camera = ctx->selected_entity;
        }
      }
    }

    if (ctx->selected_entity.hasComponent<MeshComponent>()) {
      if (ImGui::CollapsingHeader("Mesh")) {
        MeshComponent &mesh =
            ctx->selected_entity.getComponent<MeshComponent>();

        ImGui::Text("Mesh");
        ImGui::SameLine();
        if (ImGui::Button("+")) {
          ImGuiFileDialog::Instance()->OpenDialog("LoadMeshDlgKey",
                                                  "Choose File", ".obj", ".");
        }
      }
    }

    if (ctx->selected_entity.hasComponent<MeshComponent>()) {
      if (ImGui::CollapsingHeader("Material")) {
        MeshComponent &mesh =
            ctx->selected_entity.getComponent<MeshComponent>();

        static int active_material = -1;
        if (ImGui::BeginListBox("Active Materials")) {
          for (int i = 0; i < mesh.materials.size(); ++i) {
            Material *mat = MaterialManager::getMaterial(mesh.materials[i]);

            if (mat) {
              const bool selected = active_material == mesh.materials[i];
              if (ImGui::Selectable(mat->name.c_str(), selected)) {
                active_material = mesh.materials[i];

                mesh.setMaterial(active_material);
              }

              if (selected)
                ImGui::SetItemDefaultFocus();
            }
          }

          ImGui::EndListBox();
        }

        if (ImGui::Button("+"))
          ImGui::OpenPopup("add_material_popup");

        if (ImGui::BeginPopup("add_material_popup")) {
          std::vector<Material> &materials = MaterialManager::materials;
          for (int i = 0; i < materials.size(); i++) {
            Material &material = materials[i];
            if (ImGui::Selectable(materials[i].name.c_str()))
              mesh.addMaterial(i);
          }

          ImGui::EndPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("-")) {
          if (active_material != -1) {
            mesh.removeMaterial(active_material);
          }
        }
      }
    }

    if (ctx->selected_entity.hasComponent<LightComponent>()) {
      if (ImGui::CollapsingHeader("Light")) {
        LightComponent &light =
            ctx->selected_entity.getComponent<LightComponent>();

        const char *types[] = {"Spot", "Point", "Directional"};
        int type = light.type;
        float ambient[3] = {light.ambient.x, light.ambient.y, light.ambient.z};
        float diffuse[3] = {light.diffuse.x, light.diffuse.y, light.diffuse.z};
        float specular[3] = {light.specular.x, light.specular.y,
                             light.specular.z};

        if (ImGui::Combo("Type", &type, types, 3))
          light.type = (LightComponent::LightType)type;
        if (ImGui::DragFloat3("Ambient", ambient, 0.1f, 0.0f, 0.0f, "%.2f"))
          light.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
        if (ImGui::DragFloat3("Diffuse", diffuse, 0.1f, 0.0f, 0.0f, "%.2f"))
          light.diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
        if (ImGui::DragFloat3("Specular", specular, 0.1f, 0.0f, 0.0f, "%.2f"))
          light.specular = glm::vec3(specular[0], specular[1], specular[2]);

        switch (light.type) {
        case LightComponent::LightType::SPOT: {
          float cut_off = light.properties.spot_light.cut_off;
          float outer_cut_off = light.properties.spot_light.outer_cut_off;
          if (ImGui::DragFloat("Cutoff", &cut_off, 0.1f, 0.0f, 0.0f, "%.2f"))
            light.properties.spot_light.cut_off = cut_off;
          if (ImGui::DragFloat("Outer Cutoff", &outer_cut_off, 0.1f, 0.0f, 0.0f,
                               "%.2f"))
            light.properties.spot_light.outer_cut_off = outer_cut_off;
        } break;
        };
      }
    }

    if (ImGui::Button("Add Component"))
      ImGui::OpenPopup("AddComponent");
    if (ImGui::BeginPopup("AddComponent")) {
      showAddComponentPopup<CameraComponent>("Camera");
      showAddComponentPopup<MeshComponent>("Mesh");
      showAddComponentPopup<LightComponent>("Light");

      ImGui::EndPopup();
    }

    if (ImGui::Button("Remove Component"))
      ImGui::OpenPopup("RemoveComponent");
    if (ImGui::BeginPopup("RemoveComponent")) {
      showRemoveComponentPopup<CameraComponent>("Camera");
      showRemoveComponentPopup<MeshComponent>("Mesh");
      showRemoveComponentPopup<LightComponent>("Light");

      ImGui::EndPopup();
    }
  }

  ImGui::End();
}

template <typename T> void Editor::showAddComponentPopup(const char *str) {
  if (!ctx->selected_entity.hasComponent<T>()) {
    if (ImGui::MenuItem(str)) {
      ctx->selected_entity.addComponent<T>();
      ImGui::CloseCurrentPopup();
    }
  }
}

template <typename T> void Editor::showRemoveComponentPopup(const char *str) {
  if (ctx->selected_entity.hasComponent<T>()) {
    if (ImGui::MenuItem(str)) {
      ctx->selected_entity.removeComponent<T>();
      ImGui::CloseCurrentPopup();
    }
  }
}

void Editor::showCameraPanel() {

  ImGui::Begin("Editor Camera");

  EditorCamera *camera = ctx->editor_camera;

  ImGui::Checkbox("Is Orthographic", &camera->orthographic);

  float near = camera->near;
  if (ImGui::DragFloat("Near", &near, 0.1f, 0.0f, 0.0f, "%.2f"))
    camera->near = near;

  float far = camera->far;
  if (ImGui::DragFloat("Far", &far, 0.1f, 0.0f, 0.0f, "%.2f"))
    camera->far = far;

  if (!camera->orthographic) {
    float fov = camera->fov;
    if (ImGui::DragFloat("FOV", &fov, 0.1f, 0.0f, 0.0f, "%.2f"))
      camera->fov = fov;
  }

  ImGui::Text("Perpective Presets");
  if (ImGui::Button("Left")) {
    camera->pitch = 0.0f;
    camera->yaw = glm::radians(90.0f);
  }
  ImGui::SameLine();
  if (ImGui::Button("Right")) {
    camera->pitch = 0.0f;
    camera->yaw = glm::radians(-90.0f);
  }

  if (ImGui::Button("Top")) {
    camera->pitch = glm::radians(90.0f);
    camera->yaw = 0.0f;
  }
  ImGui::SameLine();
  if (ImGui::Button("Bottom")) {
    camera->pitch = glm::radians(-90.0f);
    camera->yaw = 0.0f;
  }

  if (ImGui::Button("Front")) {
    camera->pitch = 0.0f;
    camera->yaw = 0.0f;
  }
  ImGui::SameLine();
  if (ImGui::Button("Back")) {
    camera->pitch = 0.0f;
    camera->yaw = glm::radians(180.0f);
  }

  entt::registry &registry = ctx->scene->getEntityRegistry();

  ImGui::Text("Copy Scene Camera Presets");
  if (ImGui::CollapsingHeader("Scene Camera")) {
    entt::registry &registry = ctx->scene->getEntityRegistry();
    auto group = registry.group<TagComponent>(
        entt::get<CameraComponent, TransformComponent>);
    for (auto entity : group) {
      auto [tag, scene_camera, transform] =
          group.get<TagComponent, CameraComponent, TransformComponent>(entity);

      if (ImGui::Button(tag.tag.c_str())) {
        camera->near = scene_camera.near;
        camera->far = scene_camera.far;
        camera->fov = scene_camera.fov;
        /* TODO: camera->orthographic */
        camera->pitch = glm::radians(transform.rotation.x);
        camera->yaw = glm::radians(transform.rotation.y);
        camera->focal_point = scene_camera.focal_point;
        camera->distance = scene_camera.distance;
        camera->viewport_width = scene_camera.viewport_width;
        camera->viewport_height = scene_camera.viewport_height;
      }
    }
  }

  ImGui::End();
}

void Editor::showLines() {
  glm::mat4 view_mat = ctx->editor_camera->getViewMatrix();
  glm::mat4 proj_mat = ctx->editor_camera->getProjectionMatrix();
  glm::vec3 cam_pos = ctx->editor_camera->position;
  float far = ctx->editor_camera->far;

  if (ctx->active_camera) {
    auto &camera_component = ctx->active_camera.getComponent<CameraComponent>();
    auto &transform_component =
        ctx->active_camera.getComponent<TransformComponent>();
    view_mat = camera_component.getViewMatrix(transform_component.position,
                                              transform_component.rotation);
    proj_mat = camera_component.getProjectionMatrix();
    cam_pos = transform_component.position;
    far = camera_component.far;
  }

  for (float x = cam_pos.x - far; x < cam_pos.x + far; x += 0.5f) {
    glm::vec3 start = glm::vec3((int)x, 0, (int)(cam_pos.z - far));
    glm::vec3 end = glm::vec3((int)x, 0, (int)(cam_pos.z + far));
    glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
    if ((int)x == 0) {
      color = glm::vec3(1, 0.4, 0.4);
    }

    Gizmos::drawLine(view_mat, proj_mat, start, end, color);
  }

  for (float z = cam_pos.z - far; z < cam_pos.z + far; z += 0.5f) {
    glm::vec3 start = glm::vec3((int)(cam_pos.x - far), 0, (int)z);
    glm::vec3 end = glm::vec3((int)(cam_pos.x + far), 0, (int)z);
    glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
    if ((int)z == 0) {
      color = glm::vec3(0.55, 0.8, 0.9);
    }

    Gizmos::drawLine(view_mat, proj_mat, start, end, color);
  }
}
