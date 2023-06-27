#include "editor.h"

#include "../ecs/components/tag_component.h"
#include "../ecs/components/transform_component.h"
#include "../ecs/components/mesh_component.h"
#include "../ecs/components/camera_component.h"
#include "../ecs/components/light_component.h"
#include "../graphics/model.h"
#include "../graphics/renderer.h"
#include "../graphics/gizmos.h"
#include "../core/input.h"
#include "../core/time.h"
#include "editor_camera.h"
#include "../graphics/material.h"
#include "../graphics/shaders/shader_container.h"

#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "imnodes/imnodes.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

void Editor::create() {
    ctx = new EditorContext();

    receivers.push_back(createSpaceLayoutReceiver());
    receivers.push_back(createSpaceModelingReceiver());
    receivers.push_back(createSpaceShadingReceiver());
    active_receiver = receivers[0]; // Set space layout as active

    for(EventReceiver *recv : receivers) {
	recv->onCreate(ctx);
    }
    
    Model model;
    model.loadFromPath("datafiles/monkey/monkey.obj");
	
    float near = 0.1f;
    float far = 50.0f;

    ctx->scene = new Scene();    
    
    ctx->editor_camera = new EditorCamera();
    ctx->editor_camera->create(45, 1.778f, near, far);    
    
    Entity camera_entity = ctx->scene->createEntity("Camera");
    auto& camera_component = camera_entity.addComponent<CameraComponent>();        
    
    Entity object = ctx->scene->createEntity("Monkey");

    auto& mesh = object.addComponent<MeshComponent>();
    mesh.model = model;
    Material material;
    mesh.default_material = material;
	
    Entity object2 = ctx->scene->createEntity("Monkey2");
    auto& mesh2 = object2.addComponent<MeshComponent>();
    mesh2.model = model;
    auto& tr = object2.getComponent<TransformComponent>();
    tr.position = glm::vec3(3.0, 0, 0);
    tr.scale = glm::vec3(0.5, 0.5, 0.5);
    tr.rotation = glm::vec3(36, 80, 170);    

    Entity dir_light = ctx->scene->createEntity("DirectionalLight");
    auto& light_dir = dir_light.addComponent<LightComponent>();
    light_dir.type = LightComponent::LightType::DIRECTIONAL;
    auto& tr1 = dir_light.getComponent<TransformComponent>();
    tr1.rotation.x = -100;

    auto& tr2 = camera_entity.getComponent<TransformComponent>();
    tr2.position = glm::vec3(0, 0, 3);         
}

void Editor::destroy() {
    for(EventReceiver *recv : receivers) {
	recv->onDestroy(ctx);
	delete recv;
    }
    
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
    
    if(Input::wasMouseButtonHeld(SDL_BUTTON_MIDDLE)) {
	if(Input::wasKeyHeld(SDLK_LSHIFT)) {
	    if(!ctx->active_camera) {
		ctx->editor_camera->pan(mouse_delta);
	    }
	} else {	    
	    if(!ctx->active_camera) {
		ctx->editor_camera->rotate(mouse_delta);
	    }
	}
    }
    if(Input::wasWheelMoved()) {	
	if(ctx->active_camera) {
	    auto& camera_component = ctx->active_camera.getComponent<CameraComponent>();
	    auto& transform_component = ctx->active_camera.getComponent<TransformComponent>();
	    camera_component.zoom(delta_time * wheel_movement.y, transform_component.rotation);
	} else {
	    ctx->editor_camera->zoom(delta_time * wheel_movement.y);
	}
    }    
    if(Input::wasKeyPressed(SDLK_ESCAPE)) {
	if(ctx->active_camera) {
	    ctx->active_camera = {};
	}
    }

    active_receiver->onUpdate(ctx);       
}

void Editor::onDraw() {
    active_receiver->onResize(ctx);

    active_receiver->onRenderBegin(ctx);
    active_receiver->onRender(ctx);
	
    showLines();

    active_receiver->onRenderPostProcessing(ctx);
    active_receiver->onRenderEnd(ctx);
  
    static bool dockspace_open = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar |
	ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	window_flags |= ImGuiWindowFlags_NoBackground;
	
    // NOTE: dockspace begin
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);

    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    } else {
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
	ImGui::Text("Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
	ImGui::SameLine(0.0f, 0.0f);
	if (ImGui::SmallButton("click here"))
	    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    static bool show_demo = true;
    if(show_demo) ImGui::ShowDemoWindow(&show_demo);

    showMenuBar();
    showHierarchyPanel();
    showInspectorPanel();

    active_receiver->onDrawUIBegin(ctx);
    active_receiver->onDrawUI(ctx);
    active_receiver->onDrawUIEnd(ctx);

    // NOTE: dockspace end
    ImGui::End();
}

void Editor::showMenuBar() {
    if(ImGui::BeginMainMenuBar()) {
	if(ImGui::BeginMenu("File")) {
	    if(ImGui::MenuItem("New")) {

	    }
	    if(ImGui::MenuItem("Open")) {
		// ImGuiFileDialog::Instance()->OpenDialog("LoadSceneDlgKey", "Choose File", ".scene", ".");
	    }

	    ImGui::Separator();
	    if(ImGui::MenuItem("Save", "Ctrl+S")) {
		// ImGuiFileDialog::Instance()->OpenDialog("SaveSceneDlgKey", "Choose File", ".scene", ".");
	    }

	    ImGui::Separator();
	    if(ImGui::MenuItem("Import")) {}
	    if(ImGui::MenuItem("Export")) {}

	    ImGui::Separator();
	    if(ImGui::MenuItem("Quit")) {}
	    ImGui::EndMenu();
	}
	    
	if(ImGui::BeginMenu("Edit")) {
	    if(ImGui::MenuItem("Undo", "CTRL+Z")) {}
	    if(ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}
	    if(ImGui::MenuItem("Repeat Last", "SHIFT+R", false, false)) {}
				
	    ImGui::Separator();
	    if(ImGui::MenuItem("Cut", "CTRL+X")) {}
	    if(ImGui::MenuItem("Copy", "CTRL+C")) {}
	    if(ImGui::MenuItem("Paste", "CTRL+V")) {}
				
	    ImGui::Separator();
	    if(ImGui::BeginMenu("Preferences")) {
		// TODO: open another window
	    }
	    ImGui::EndMenu();
	}

	if(ImGui::BeginMenu("Window")) {
	    if(ImGui::MenuItem("New Window")) {}
	    if(ImGui::MenuItem("Toggle Window Fullscreen")) {}

	    ImGui::Separator();
	    if(ImGui::MenuItem("Next Workspace")) {}
	    if(ImGui::MenuItem("Previous Workspace")) {}
				
	    ImGui::EndMenu();
	}

	if(ImGui::BeginTabBar("EditorTabBar")) {
	    if(ImGui::BeginTabItem("Layout")) {
		active_receiver = receivers[0];
		ImGui::EndTabItem();
	    }
	    if(ImGui::BeginTabItem("Modeling")) {
		active_receiver = receivers[1];
		ImGui::EndTabItem();
	    }
	    if(ImGui::BeginTabItem("Shading")) {
		active_receiver = receivers[2];
		ImGui::EndTabItem();
	    }
      
	    ImGui::EndTabBar();
	}    
    
	ImGui::EndMainMenuBar();
    }

    // if(ImGuiFileDialog::Instance()->Display("LoadSceneDlgKey"))  {
    // 	if(ImGuiFileDialog::Instance()->IsOk()) {
    // 		std::string fp = ImGuiFileDialog::Instance()->GetFilePathName();
    // 		std::string name = ImGuiFileDialog::Instance()->GetCurrentFileName();
    // 	}

    // 	ImGuiFileDialog::Instance()->Close();
    // }

    // if(ImGuiFileDialog::Instance()->Display("SaveSceneDlgKey"))  {
    // 	if(ImGuiFileDialog::Instance()->IsOk()) {
    // 		std::string fp = ImGuiFileDialog::Instance()->GetFilePathName();

    // 	}

    // 	ImGuiFileDialog::Instance()->Close();
    // }

    if(ImGuiFileDialog::Instance()->Display("LoadMeshDlgKey"))  {
	if(ImGuiFileDialog::Instance()->IsOk()) {
	    std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
	    std::string name = ImGuiFileDialog::Instance()->GetCurrentFileName();

	    if(ctx->selected_entity) {
		if(ctx->selected_entity.hasComponent<MeshComponent>()) {
		    MeshComponent& mesh = ctx->selected_entity.getComponent<MeshComponent>();
		    mesh.model.loadFromPath(path.c_str());
		}
	    }
	}

	ImGuiFileDialog::Instance()->Close();
    }
}

void Editor::showHierarchyPanel() {
    ImGui::Begin("Hierarchy");
    if(ImGui::CollapsingHeader("Scene")) {
	entt::registry& registry = ctx->scene->getEntityRegistry();
	auto view = registry.view<TagComponent>();
	for(auto entity : view) {
	    auto& tag = view.get<TagComponent>(entity);
	    if(ImGui::Selectable(tag.tag.c_str(), ctx->selected_entity == entity))
		ctx->selected_entity.create(entity, ctx->scene);
	}
    }

    if(ImGui::BeginPopupContextWindow()) {
	if(ImGui::MenuItem("Create Entity")) {
	    ctx->scene->createEntity();
	}

	if(ctx->selected_entity) {
	    if(ImGui::MenuItem("Destoy Entity")) {
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

    if(ctx->selected_entity) {
	if(ctx->selected_entity.hasComponent<TagComponent>()) {
	    ImGui::Text("Tag");
	    TagComponent& tag = ctx->selected_entity.getComponent<TagComponent>();
	    char buff[256];
	    memset(buff, 0, sizeof(buff));
	    strncpy(buff, tag.tag.c_str(), sizeof(buff));
	    if(ImGui::InputText("##Tag", buff, sizeof(buff))) {
		tag.tag = std::string(buff);
	    }
	}

	if(ctx->selected_entity.hasComponent<TransformComponent>()) {
	    if(ImGui::CollapsingHeader("Transform")) {
		TransformComponent& transform = ctx->selected_entity.getComponent<TransformComponent>();

		float p[3] = { transform.position.x, transform.position.y, transform.position.z };
		float s[3] = { transform.scale.x, transform.scale.y, transform.scale.z };
		float r[3] = { transform.rotation.x, transform.rotation.y, transform.rotation.z };

		if(ImGui::DragFloat3("Position", p, 0.1f, 0.0f, 0.0f, "%.2f"))
		    transform.position = glm::vec3(p[0], p[1], p[2]);		
		if(ImGui::DragFloat3("Scale", s, 0.1f, 0.0f, 0.0f, "%.2f"))
		    transform.scale = glm::vec3(s[0], s[1], s[2]);
		if(ImGui::DragFloat3("Rotation", r, 0.1f, 0.0f, 0.0f, "%.2f"))
		    transform.rotation = glm::vec3(r[0], r[1], r[2]);
	    }
	}

	if(ctx->selected_entity.hasComponent<CameraComponent>()) {
	    if(ImGui::CollapsingHeader("Camera")) {
		if(ImGui::Button("Select as active")) {		    
		    ctx->active_camera = ctx->selected_entity;
		}		    		
	    }
	}

	if(ctx->selected_entity.hasComponent<MeshComponent>()) {
	    if(ImGui::CollapsingHeader("Mesh")) {
		MeshComponent& mesh = ctx->selected_entity.getComponent<MeshComponent>();

		ImGui::Text("Mesh");
		ImGui::SameLine();
		if(ImGui::Button("+")) {
		    ImGuiFileDialog::Instance()->OpenDialog("LoadMeshDlgKey", "Choose File", ".obj", ".");
		}
	    }
	}

	if(ctx->selected_entity.hasComponent<LightComponent>()) {
	    if(ImGui::CollapsingHeader("Light")) {
		LightComponent& light = ctx->selected_entity.getComponent<LightComponent>();

		const char* types[] = { "Spot", "Point", "Directional" };
		int type = light.type;
		float ambient[3] = { light.ambient.x, light.ambient.y, light.ambient.z };
		float diffuse[3] = { light.diffuse.x, light.diffuse.y, light.diffuse.z };
		float specular[3] = { light.specular.x, light.specular.y, light.specular.z };
				
		if(ImGui::Combo("Type", &type, types, 3))
		    light.type = (LightComponent::LightType)type;
		if(ImGui::DragFloat3("Ambient", ambient, 0.1f, 0.0f, 0.0f, "%.2f"))
		    light.ambient = glm::vec3(ambient[0], ambient[1], ambient[2]);
		if(ImGui::DragFloat3("Diffuse", diffuse, 0.1f, 0.0f, 0.0f, "%.2f"))
		    light.diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
		if(ImGui::DragFloat3("Specular", specular, 0.1f, 0.0f, 0.0f, "%.2f"))
		    light.specular = glm::vec3(specular[0], specular[1], specular[2]);
		
		switch(light.type) {
		case LightComponent::LightType::POINT: {
		    float constant = light.properties.point_light.constant;
		    float linear = light.properties.point_light.linear;
		    float quadratic = light.properties.point_light.quadratic;
		    if(ImGui::DragFloat("Constant", &constant, 0.1f, 0.0f, 0.0f, "%.2f"))
			light.properties.point_light.constant = constant;
		    if(ImGui::DragFloat("Linear", &linear, 0.1f, 0.0f, 0.0f, "%.2f"))
			light.properties.point_light.linear = linear;
		    if(ImGui::DragFloat("Quadratic", &quadratic, 0.1f, 0.0f, 0.0f, "%.2f"))
			light.properties.point_light.quadratic = quadratic;
		} break;
		case LightComponent::LightType::SPOT: {
		    float cut_off = light.properties.spot_light.cut_off;
		    float outer_cut_off = light.properties.spot_light.outer_cut_off;
		    float constant = light.properties.spot_light.constant;
		    float linear = light.properties.spot_light.linear;
		    float quadratic = light.properties.spot_light.quadratic;
		    if(ImGui::DragFloat("Constant", &constant, 0.1f, 0.0f, 0.0f, "%.2f"))
			light.properties.spot_light.constant = constant;
		    if(ImGui::DragFloat("Linear", &linear, 0.1f, 0.0f, 0.0f, "%.2f"))
			light.properties.spot_light.linear = linear;
		    if(ImGui::DragFloat("Quadratic", &quadratic, 0.1f, 0.0f, 0.0f, "%.2f"))
			light.properties.spot_light.quadratic = quadratic;
		} break;
		};
	    }
	}

	if(ImGui::Button("Add Component"))
	    ImGui::OpenPopup("AddComponent");
	if(ImGui::BeginPopup("AddComponent")) {
	    showAddComponentPopup<TransformComponent>("Transform");
	    showAddComponentPopup<CameraComponent>("Camera");
	    showAddComponentPopup<MeshComponent>("Mesh");
	    showAddComponentPopup<LightComponent>("Light");
		
	    ImGui::EndPopup();
	}

	if(ImGui::Button("Remove Component"))
	    ImGui::OpenPopup("RemoveComponent");
	if(ImGui::BeginPopup("RemoveComponent")) {
	    showRemoveComponentPopup<TransformComponent>("Transform");
	    showRemoveComponentPopup<CameraComponent>("Camera");
	    showRemoveComponentPopup<MeshComponent>("Mesh");
	    showRemoveComponentPopup<LightComponent>("Light");
		
	    ImGui::EndPopup();
	}
    }

    ImGui::End();
}

template<typename T> void Editor::showAddComponentPopup(const char* str) {
    if(!ctx->selected_entity.hasComponent<T>()) {
	if(ImGui::MenuItem(str)) {
	    ctx->selected_entity.addComponent<T>();
	    ImGui::CloseCurrentPopup();
	}
    }
}

template<typename T> void Editor::showRemoveComponentPopup(const char* str) {
    if (ctx->selected_entity.hasComponent<T>()) {
	if (ImGui::MenuItem(str)) {
	    ctx->selected_entity.removeComponent<T>();
	    ImGui::CloseCurrentPopup();
	}
    }
}

void Editor::showLines() {
    glm::mat4 view_mat = ctx->editor_camera->getViewMatrix();
    glm::mat4 proj_mat = ctx->editor_camera->getProjectionMatrix();    
    glm::vec3 cam_pos = ctx->editor_camera->position;
    float far = ctx->editor_camera->far;
    
    if(ctx->active_camera) {
	auto& camera_component = ctx->active_camera.getComponent<CameraComponent>();
	auto& transform_component = ctx->active_camera.getComponent<TransformComponent>();
	view_mat = camera_component.getViewMatrix(transform_component.position, transform_component.rotation);
	proj_mat = camera_component.getProjectionMatrix();
	cam_pos = transform_component.position;
	far = camera_component.far;
    }
           
    for(float x = cam_pos.x - far; x < cam_pos.x + far; x += 0.5f) {
	glm::vec3 start = glm::vec3((int)x, 0, (int)(cam_pos.z - far));
	glm::vec3 end = glm::vec3((int)x, 0, (int)(cam_pos.z + far));
	glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
	if((int)x == 0) {
	    color = glm::vec3(1, 0.4, 0.4);
	}

	Gizmos::drawLine(view_mat, proj_mat, start, end, color);
    }

    for(float z = cam_pos.z - far; z < cam_pos.z + far; z += 0.5f) {
	glm::vec3 start = glm::vec3((int)(cam_pos.x - far), 0, (int)z);
	glm::vec3 end = glm::vec3((int)(cam_pos.x + far), 0, (int)z);
	glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
	if((int)z == 0) {
	    color = glm::vec3(0.55, 0.8, 0.9);
	}

	Gizmos::drawLine(view_mat, proj_mat, start, end, color);
    }
}
