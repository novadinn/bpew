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

#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "imnodes/imnodes.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

void Editor::create() {
    Model model;
    model.loadFromPath("datafiles/monkey/monkey.obj");
	
    float near = 0.1f;
    float far = 50.0f;
	
    scene = new Scene();
    editor_camera = scene->createEntity("Camera");
    auto& camera_component = editor_camera.addComponent<CameraComponent>();    
    camera_component.camera.create(45, 1.778f, near, far);
    camera_component.editor = true;
	
    Entity object = scene->createEntity("Monkey");
    auto& mesh = object.addComponent<MeshComponent>();
    mesh.model = model;
	
    Entity object2 = scene->createEntity("Monkey2");
    auto& mesh2 = object2.addComponent<MeshComponent>();
    mesh2.model = model;
    auto& tr = object2.getComponent<TransformComponent>();
    tr.position = glm::vec3(3.0, 0, 0);
    tr.scale = glm::vec3(0.5, 0.5, 0.5);
    tr.rotation = glm::vec3(36, 80, 170);

    Entity dir_light = scene->createEntity("DirectionalLight");
    auto& light_dir = dir_light.addComponent<LightComponent>();
    light_dir.type = LightComponent::LightType::DIRECTIONAL;
    auto& tr1 = dir_light.getComponent<TransformComponent>();
    tr1.rotation.x = -100;
	
    FramebufferData data;
    data.formats = {GL_RGBA8, GL_RGBA32F, GL_DEPTH24_STENCIL8};
    data.width = 800;
    data.height = 800;
    data.samples = 1;
    framebuffer.create(data);

    data.formats = {GL_RGBA8};
    pp_framebuffer.create(data);
}

void Editor::destroy() {
    delete scene;
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

    auto& camera_component = editor_camera.getComponent<CameraComponent>();
    
    if(Input::wasMouseButtonHeld(SDL_BUTTON_MIDDLE)) {
	if(Input::wasKeyHeld(SDLK_LSHIFT)) {
	    camera_component.camera.pan(mouse_delta);
	} else {
	    camera_component.camera.rotate(mouse_delta);
	}
    }
    if(Input::wasMouseButtonPressed(SDL_BUTTON_LEFT)) {
	if(viewport_hovered && !ImGuizmo::IsOver())
	    selected_entity = hovered_entity;
    }
    if(Input::wasWheelMoved()) {
	camera_component.camera.zoom(delta_time * wheel_movement.y);
    }

    if(Input::wasKeyPressed(SDLK_q)) {
	gizmo_operation = -1;
    } else if(Input::wasKeyPressed(SDLK_t)) {
	gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
    } else if(Input::wasKeyPressed(SDLK_r)) {
	gizmo_operation = ImGuizmo::OPERATION::ROTATE;
    } else if(Input::wasKeyPressed(SDLK_s)) {
	gizmo_operation = ImGuizmo::OPERATION::SCALE;
    }

    // TODO: create imgui buttons instead of this
    if(Input::wasKeyHeld(SDLK_z)) {
	if(Input::wasKeyPressed(SDLK_1)) {
	    draw_mode = DrawMode::WIREFRAME;
	} else if(Input::wasKeyPressed(SDLK_2)) {
	    draw_mode = DrawMode::RENDERED;
	} else if(Input::wasKeyPressed(SDLK_3)) {
	    draw_mode = DrawMode::SOLID;
	} else if(Input::wasKeyPressed(SDLK_4)) {
	    draw_mode = DrawMode::MATERIAL_PREVIEW;
	}
    }

    scene->onUpdate();
}

void Editor::onDraw() {
    scene->onResize(viewport_size.x, viewport_size.y);

    FramebufferData data = framebuffer.getFramebufferData();
    if(viewport_size.x > 0.0f && viewport_size.y > 0.0f &&
       (data.width != viewport_size.x || data.height != viewport_size.y)) {
		
	framebuffer.resize(viewport_size.x, viewport_size.y);
	pp_framebuffer.resize(viewport_size.x, viewport_size.y);
    }

    framebuffer.bind();
    Renderer::clear();
    framebuffer.clearRGBA8ColorAttachment(1, glm::vec4(-1, 0, 0, 0));
	
    switch(draw_mode) {
    case DrawMode::WIREFRAME: {
	scene->onDrawWireframe();
    } break;
    case DrawMode::RENDERED: {
	scene->onDrawRendered();
    } break;
    case DrawMode::SOLID: {
	scene->onDrawSolid();
    } break;
    case DrawMode::MATERIAL_PREVIEW: {
	scene->onDrawMaterialPreview();
    } break;
    }

    auto[mx, my] = ImGui::GetMousePos();
    mx -= viewport_bounds[0].x;
    my -= viewport_bounds[0].y;
    glm::vec2 viewport_size = viewport_bounds[1] - viewport_bounds[0];
    my = viewport_size.y - my;
    int mouse_x = (int)mx;
    int mouse_y = (int)my;

    if (mouse_x >= 0 && mouse_y >= 0 &&
	mouse_x < (int)viewport_size.x && mouse_y < (int)viewport_size.y) {

	framebuffer.bindReadAttachment(1);
			
	glm::vec4 pixel_data = framebuffer.readRGBA8Pixel(mouse_x, mouse_y);
	int id = (int)pixel_data.x;
	if(id != -1) {
	    Entity entity;
	    entity.create((entt::entity)id, scene);
	    hovered_entity = entity;
	} else {
	    hovered_entity = {};
	}
			
	framebuffer.bindReadAttachment(0);
    }
	
    showLines();
  
    framebuffer.unbind();

    pp_framebuffer.bind();
    Renderer::clear();
    Renderer::outlineSelectedMesh(framebuffer.getColorAttachmentID(0),
				  framebuffer.getColorAttachmentID(1),
				  (uint32)selected_entity,
				  glm::vec3(0.0f, 0.0f, 1.0f), 0.5f);
    pp_framebuffer.unbind();
  
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

    // NOTE: dockspace end
    ImGui::End();
}

void Editor::showLayoutSpace() {    
    space_layout.begin();

    viewport_hovered = isViewportHovered();
    viewport_size = getAvailableViewportSize();
    viewport_bounds[0] = getAvailableViewportBoundsMin();
    viewport_bounds[1] = getAvailableViewportBoundsMax();

    space_layout.drawImage(pp_framebuffer.getColorAttachmentID(0), viewport_size);

    auto& camera_component = editor_camera.getComponent<CameraComponent>();
    
    if(selected_entity && gizmo_operation != -1 &&
       selected_entity.hasComponent<TransformComponent>()) {
	TransformComponent& transform = selected_entity.getComponent<TransformComponent>();	
	glm::mat4 view = camera_component.camera.getViewMatrix();
	glm::mat4 projection = camera_component.camera.getProjectionMatrix();
	bool snap = Input::wasKeyHeld(SDLK_LCTRL);
	
	space_layout.manipulateSelected(transform, view, projection,
					 gizmo_operation, snap);
    }
	
    space_layout.end();
}

void Editor::showModelingSpace() {
    space_modeling.begin();

    viewport_hovered = isViewportHovered();
    viewport_size = getAvailableViewportSize();
    viewport_bounds[0] = getAvailableViewportBoundsMin();
    viewport_bounds[1] = getAvailableViewportBoundsMax();

    space_modeling.drawImage(pp_framebuffer.getColorAttachmentID(0), viewport_size);

    auto& camera_component = editor_camera.getComponent<CameraComponent>();
    
    if(selected_entity && gizmo_operation != -1 &&
       selected_entity.hasComponent<TransformComponent>()) {
	TransformComponent& transform = selected_entity.getComponent<TransformComponent>();	
	glm::mat4 view = camera_component.camera.getViewMatrix();
	glm::mat4 projection = camera_component.camera.getProjectionMatrix();
	bool snap = Input::wasKeyHeld(SDLK_LCTRL);
	
	space_modeling.manipulateSelected(transform, view, projection,
					gizmo_operation, snap);
    }
	
    space_modeling.end();
}

void Editor::showShadingSpace() {
    space_shading.begin();

    space_shading.drawHeader(selected_entity);
    space_shading.drawNodeEditor(selected_entity);
    
    space_shading.end();
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

	// Render tabs
	if(ImGui::BeginTabBar("EditorTabBar")) {
	    if(ImGui::BeginTabItem("Layout")) {
		showLayoutSpace();
		ImGui::EndTabItem();
	    }
	    if(ImGui::BeginTabItem("Modeling")) {
		showModelingSpace();
		ImGui::EndTabItem();
	    }
	    if(ImGui::BeginTabItem("Shading")) {
		showShadingSpace();
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

	    if(selected_entity) {
		if(selected_entity.hasComponent<MeshComponent>()) {
		    MeshComponent& mesh = selected_entity.getComponent<MeshComponent>();
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
	entt::registry& registry = scene->getEntityRegistry();
	auto view = registry.view<TagComponent>();
	for(auto entity : view) {
	    auto& tag = view.get<TagComponent>(entity);
	    if(ImGui::Selectable(tag.tag.c_str(), selected_entity == entity))
		selected_entity.create(entity, scene);
	}
    }

    if(ImGui::BeginPopupContextWindow()) {
	if(ImGui::MenuItem("Create Entity")) {
	    scene->createEntity();
	}

	if(selected_entity) {
	    if(ImGui::MenuItem("Destoy Entity")) {
		scene->destroyEntity(selected_entity);
		selected_entity = {};
	    }
	}

	ImGui::EndPopup();
    }

    ImGui::End();
}

void Editor::showInspectorPanel() {
    ImGui::Begin("Inspector");

    if(selected_entity) {
	if(selected_entity.hasComponent<TagComponent>()) {
	    ImGui::Text("Tag");
	    TagComponent& tag = selected_entity.getComponent<TagComponent>();
	    char buff[256];
	    memset(buff, 0, sizeof(buff));
	    strncpy(buff, tag.tag.c_str(), sizeof(buff));
	    if(ImGui::InputText("##Tag", buff, sizeof(buff))) {
		tag.tag = std::string(buff);
	    }
	}

	if(selected_entity.hasComponent<TransformComponent>()) {
	    if(ImGui::CollapsingHeader("Transform")) {
		TransformComponent& transform = selected_entity.getComponent<TransformComponent>();

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

	if(selected_entity.hasComponent<CameraComponent>()) {
	    if(ImGui::CollapsingHeader("Camera")) {
		CameraComponent& camera = selected_entity.getComponent<CameraComponent>();
		Camera& cam = camera.camera;

		// TODO: should we care about camera's position/rotation?
				
		float fov = cam.fov;
		float near = cam.near;
		float far = cam.far;

		if(ImGui::DragFloat("FOV", &fov, 0.1f, 0.0f, 0.0f, "%.2f"))
		    cam.fov = fov;
		if(ImGui::DragFloat("Near", &near, 0.1f, 0.1f, FLT_MAX, "%.2f"))
		    cam.near = near;
		if(ImGui::DragFloat("Far", &far, 0.1f, 0.1f, FLT_MAX, "%.2f"))
		    cam.far = far;
	    }
	}

	if(selected_entity.hasComponent<MeshComponent>()) {
	    if(ImGui::CollapsingHeader("Mesh")) {
		MeshComponent& mesh = selected_entity.getComponent<MeshComponent>();

		ImGui::Text("Mesh");
		ImGui::SameLine();
		if(ImGui::Button("+")) {
		    ImGuiFileDialog::Instance()->OpenDialog("LoadMeshDlgKey", "Choose File", ".obj", ".");
		}
	    }
	}

	if(selected_entity.hasComponent<LightComponent>()) {
	    if(ImGui::CollapsingHeader("Light")) {
		LightComponent& light = selected_entity.getComponent<LightComponent>();

		const char* types[] = { "Spot", "Point", "Directional" };
		int type = light.type;
		float color[3] = { light.color.x, light.color.y, light.color.z };
		float intensity = light.intensity;
		float intensity_multiplier = light.intensity_multiplier;
				
		if(ImGui::Combo("Type", &type, types, 3))
		    light.type = (LightComponent::LightType)type;
		if(ImGui::DragFloat3("Color", color, 0.1f, 0.0f, 0.0f, "%.2f"))
		    light.color = glm::vec3(color[0], color[1], color[2]);
		if(ImGui::DragFloat("Intensity", &intensity, 0.1f, 0.0f, 0.0f, "%.2f"))
		    light.intensity = intensity;
		if(ImGui::DragFloat("Intensity Multiplier", &intensity_multiplier, 0.1f, 0.0f, 0.0f, "%.2f"))
		    light.intensity_multiplier = intensity_multiplier;
		switch(light.type) {
		case LightComponent::LightType::SPOT: {
		    float spot_angle = light.spot_angle;
		    if(ImGui::DragFloat("Spot Angle", &spot_angle, 0.1f, 0.0f, 0.0f, "%.2f"))
			light.spot_angle = spot_angle;
		} break;
		case LightComponent::LightType::POINT: {
		    float range = light.range;
		    if(ImGui::DragFloat("Range", &range, 0.1f, 0.0f, 0.0f, "%.2f"))
			light.range = range;
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
    if(!selected_entity.hasComponent<T>()) {
	if(ImGui::MenuItem(str)) {
	    selected_entity.addComponent<T>();
	    ImGui::CloseCurrentPopup();
	}
    }
}

template<typename T> void Editor::showRemoveComponentPopup(const char* str) {
    if (selected_entity.hasComponent<T>()) {
	if (ImGui::MenuItem(str)) {
	    selected_entity.removeComponent<T>();
	    ImGui::CloseCurrentPopup();
	}
    }
}

void Editor::showLines() {       
    auto& camera_component = editor_camera.getComponent<CameraComponent>();
    auto& camera_transform  = editor_camera.getComponent<TransformComponent>();
    
    glm::vec3 cam_pos = camera_transform.position;
    
    float far = camera_component.camera.far;
    for(float x = cam_pos.x - far; x < cam_pos.x + far; x += 0.5f) {
	glm::vec3 start = glm::vec3((int)x, 0, (int)(cam_pos.z - far));
	glm::vec3 end = glm::vec3((int)x, 0, (int)(cam_pos.z + far));
	glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
	if((int)x == 0) {
	    color = glm::vec3(1, 0.4, 0.4);
	}
	Gizmos::drawLine(camera_component.camera, start, end, color);
    }

    for(float z = cam_pos.z - far; z < cam_pos.z + far; z += 0.5f) {
	glm::vec3 start = glm::vec3((int)(cam_pos.x - far), 0, (int)z);
	glm::vec3 end = glm::vec3((int)(cam_pos.x + far), 0, (int)z);
	glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
	if((int)z == 0) {
	    color = glm::vec3(0.55, 0.8, 0.9);
	}
	Gizmos::drawLine(camera_component.camera, start, end, color);
    }
}

glm::vec2 Editor::getAvailableViewportSize() const {
    ImVec2 vpsize = ImGui::GetContentRegionAvail();
    
    return {vpsize.x, vpsize.y};
}

glm::vec2 Editor::getAvailableViewportBoundsMin() const {
    auto viewport_min_region = ImGui::GetWindowContentRegionMin();
    auto viewport_max_region = ImGui::GetWindowContentRegionMax();
    auto viewport_offset = ImGui::GetWindowPos();

    return {viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y};
}

glm::vec2 Editor::getAvailableViewportBoundsMax() const {
    auto viewport_min_region = ImGui::GetWindowContentRegionMin();
    auto viewport_max_region = ImGui::GetWindowContentRegionMax();
    auto viewport_offset = ImGui::GetWindowPos();
    
    return {viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y};
}

bool Editor::isViewportHovered() const {
    return ImGui::IsWindowHovered();    
}
