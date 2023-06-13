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
    Entity camera_entity = scene->createEntity("Camera");
    auto& camera_component = camera_entity.addComponent<CameraComponent>();
    main_camera = &camera_component.camera;
    main_camera->create(45, 1.778f, near, far);
    camera_component.main = true;
	
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
		
    if(Input::wasMouseButtonHeld(SDL_BUTTON_MIDDLE)) {
	if(Input::wasKeyHeld(SDLK_LSHIFT)) {
	    main_camera->pan(mouse_delta);
	} else {
	    main_camera->rotate(mouse_delta);
	}
    }
    if(Input::wasMouseButtonPressed(SDL_BUTTON_LEFT)) {
	if(viewport_hovered && !ImGuizmo::IsOver())
	    selected_entity = hovered_entity;
    }
    if(Input::wasWheelMoved()) {
	main_camera->zoom(delta_time * wheel_movement.y);
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

void Editor::showViewSpace() {    
    ImGui::Begin("Layout");
    viewport_hovered = ImGui::IsWindowHovered();
	
    ImVec2 vpsize = ImGui::GetContentRegionAvail();
    viewport_size = glm::vec2(vpsize.x, vpsize.y);

    auto viewport_min_region = ImGui::GetWindowContentRegionMin();
    auto viewport_max_region = ImGui::GetWindowContentRegionMax();
    auto viewport_offset = ImGui::GetWindowPos();
    viewport_bounds[0] = { viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y };
    viewport_bounds[1] = { viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y };
	
    uint fbid = pp_framebuffer.getColorAttachmentID(0);
    ImGui::Image(reinterpret_cast<void*>(fbid), ImVec2{ viewport_size.x, viewport_size.y },
		 ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	
    if(selected_entity && gizmo_operation != -1 &&
       selected_entity.hasComponent<TransformComponent>()) {

	glm::mat4 view = main_camera->getViewMatrix();
	glm::mat4 projection = main_camera->getProjectionMatrix();
		
	TransformComponent& transform = selected_entity.getComponent<TransformComponent>();
	glm::mat4 model = transform.getModelMatrix();

	bool snap = Input::wasKeyHeld(SDLK_LCTRL);

	Gizmos::drawManupilations((ImGuizmo::OPERATION)gizmo_operation,
				  glm::value_ptr(view), glm::value_ptr(projection),
				  glm::value_ptr(model), snap);
		
	if(ImGuizmo::IsUsing()) {
	    float* model_ptr = glm::value_ptr(model);
	    float scale_result[3];
	    float rotation_result[3];
	    float translation_result[3];
	    ImGuizmo::DecomposeMatrixToComponents(model_ptr, translation_result,
						  rotation_result, scale_result);

	    transform.position = {translation_result[0], translation_result[1], translation_result[2]};
	    transform.rotation = {rotation_result[0], rotation_result[1], rotation_result[2]};
	    transform.scale = {scale_result[0], scale_result[1], scale_result[2]};
	}
    }
	
    ImGui::End();
}

void Editor::showNodeSpace() {
    ImGui::Begin("Shading");
  
    if (ImGui::Button("Material...")) {
	if(selected_entity && selected_entity.hasComponent<MeshComponent>()) {
	    ImGui::OpenPopup("MaterialsMenu");
	} else {
	    // TODO: error message popup
	}
    }
  
    if(ImGui::BeginPopup("MaterialsMenu")) {
	MeshComponent& mesh = selected_entity.getComponent<MeshComponent>();
	std::vector<Material>& materials = mesh.materials;

	static char material_name[255];
	if(ImGui::InputText("New Material", material_name, 255,
			    ImGuiInputTextFlags_EnterReturnsTrue)) {
	    Material mat;
	    mat.name = material_name;
	    materials.push_back(mat);

	    memset(material_name, '\0', sizeof(material_name));
	}
	if(ImGui::Button("Destroy Material")) {
	    if(mesh.validMaterialIndex()) {
		materials.erase(materials.begin() + mesh.active_material_index);

		if(mesh.active_material_index == materials.size()) {
		    --mesh.active_material_index;
		}
	    }
	}
    
	if(ImGui::CollapsingHeader("Materials")) {
	    for(int i = 0; i < materials.size(); ++i) {
		Material& mat = materials[i];
	
		if(ImGui::Selectable(mat.name.c_str(), mesh.active_material_index == i, ImGuiSelectableFlags_DontClosePopups)) {
		    mesh.active_material_index = i;
		}
	    }
	}

	ImGui::EndPopup();
    }

    ImNodes::BeginNodeEditor();
  
    if(selected_entity && selected_entity.hasComponent<MeshComponent>()) {
	MeshComponent& mesh = selected_entity.getComponent<MeshComponent>();

	Material* mat = mesh.getActiveMaterial();
	if(mat) {
	    for(int i = 0; i < mat->nodes.size(); ++i) {
		Node& node = mat->nodes[i];
		std::vector<NodeProperty>& outputs = node.outputs;
		std::vector<NodeProperty>& inputs = node.inputs;

		ImNodes::BeginNode(node.id.id);
	
		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted(node.id.name.c_str());
		ImNodes::EndNodeTitleBar();
	
		// Outputs
		for(int j = 0; j < outputs.size(); ++j) {
		    NodeProperty& prop = outputs[j];
	  
		    ImNodes::BeginOutputAttribute(prop.id.id);
		    ImGui::Text("%s", prop.id.name.c_str());
		    ImNodes::EndOutputAttribute();
		}

		// Inputs
		for(int j = 0; j < inputs.size(); ++j) {
		    NodeProperty& prop = inputs[j];

		    ImNodes::BeginInputAttribute(prop.id.id);
		    ImGui::Text("%s", prop.id.name.c_str());
		    ImNodes::EndInputAttribute();
		}
	
		ImNodes::EndNode();
	    }
	}
    }
  
    // TODO: not all of those are set right now
    if(ImGui::BeginPopupContextWindow()) {
	if(ImGui::BeginMenu("Add")) {
	    if(ImGui::BeginMenu("Input")) {
		if(ImGui::Button("RGB")) {
		    createNode(NodeType::RGB);
		}
	
		ImGui::EndMenu();
	    }
	    if(ImGui::BeginMenu("Output")) {
		if(ImGui::Button("Material Output")) {
		    createNode(NodeType::MATERIAL_OUTPUT);
		}
	
		ImGui::EndMenu();
	    }
	    if(ImGui::BeginMenu("Shader")) {
	
		ImGui::EndMenu();
	    }
	    if(ImGui::BeginMenu("Texture")) {
	
		ImGui::EndMenu();
	    }
	    if(ImGui::BeginMenu("Color")) {
	
		ImGui::EndMenu();
	    }
	    if(ImGui::BeginMenu("Vector")) {
	
		ImGui::EndMenu();
	    }
	    if(ImGui::BeginMenu("Converter")) {
	
		ImGui::EndMenu();
	    }

	    ImGui::EndMenu();
	}

	if(ImGui::BeginMenu("Node")) {
	    if(ImGui::Button("Delete")) {
	
	    }
	    if(ImGui::Button("Duplicate")) {
	
	    }
	    if(ImGui::Button("Copy")) {
	
	    }
	    if(ImGui::Button("Paste")) {
	
	    }

	    ImGui::Separator();
      
	    if(ImGui::Button("Resize")) {
	
	    }
	    if(ImGui::Button("Move")) {
	
	    }
      
	    ImGui::EndMenu();
	}
    
	ImGui::EndPopup();
    }

    // Links should be drawn after the nodes
    if(selected_entity && selected_entity.hasComponent<MeshComponent>()) {
	MeshComponent& mesh = selected_entity.getComponent<MeshComponent>();
	Material* mat = mesh.getActiveMaterial();
    
	if(mat) {
	    for(int i = 0; i < mat->nodes.size(); ++i) {
		Node& node = mat->nodes[i];
		// Draw Link only from output nodes - dont need to do this twice
		std::vector<NodeProperty>& outputs = node.outputs;

		for(int j = 0; j < outputs.size(); ++j) {
		    NodeProperty& prop = outputs[j];
		    std::vector<NodeLink>& links = prop.links;

		    for(int k = 0; k < links.size(); ++k) {
			int link_id = links[k].id.id;
			int output_id = links[k].output->id.id;
			int input_id = links[k].input->id.id;

			ImNodes::Link(link_id, output_id, input_id);
		    }
		}
	    }
	}
    }
  
    ImNodes::EndNodeEditor();

    int output_id, input_id;
    NodeProperty *output_prop = nullptr, *input_prop = nullptr;

    if (ImNodes::IsLinkCreated(&output_id, &input_id)) {
	// NOTE: assumes that material and mesh are there
	MeshComponent& mesh = selected_entity.getComponent<MeshComponent>();
	Material *mat = mesh.getActiveMaterial();
	std::vector<Node>& nodes = mat->nodes;

	// TODO: slow linear search
	for(int i = 0; i < nodes.size(); ++i) {
	    Node& node = nodes[i];
	    std::vector<NodeProperty>& node_outputs = node.outputs;
	    std::vector<NodeProperty>& node_inputs = node.inputs;

	    for(int j = 0; j < node_outputs.size(); ++j) {
		NodeProperty* prop = &node_outputs[j];

		if(prop->id.id == output_id) {
		    output_prop = prop;
		}
	    }

	    for(int j = 0; j < node_inputs.size(); ++j) {
		NodeProperty* prop = &node_inputs[j];
	
		if(prop->id.id == input_id) {
		    input_prop = prop;
		}
	    }

	    if(output_prop && input_prop) {
		break;
	    }
      
	}
    }

    if(output_prop && input_prop) {
	// TODO: should we add this to both input and output node or not?
	NodeLink link;
	link.create(output_prop, input_prop);
    
	output_prop->links.push_back(link);
	input_prop->links.push_back(link);
    }

    // TODO: ImNodes::IsLinkDestroyed is not impelemted on the imnodes side (or thats just me stupid),
    // so check for deletion by our own
    int num_selected = ImNodes::NumSelectedLinks();
    // TODO: dont check for input in here - use update method
    if(num_selected > 0 && Input::wasKeyReleased(SDLK_DELETE)) {
	int selected_links[num_selected];
	ImNodes::GetSelectedLinks(&selected_links[0]);

	// TODO: slow linear search
	for(int i = 0; i < num_selected; ++i) {
	    int destroyed_link_id = selected_links[i];
	    MeshComponent& mesh = selected_entity.getComponent<MeshComponent>();
	    Material *mat = mesh.getActiveMaterial();
	    std::vector<Node>& nodes = mat->nodes;
      
	    for(int j = 0; j < nodes.size(); ++j) {
		Node& node = nodes[j];
		std::vector<NodeProperty>& node_outputs = node.outputs;
		std::vector<NodeProperty>& node_inputs = node.inputs;

		for(int k = 0; k < node_outputs.size(); ++k) {
		    NodeProperty* prop = &node_outputs[k];
		    std::vector<NodeLink>& links = prop->links;

		    for(int q = 0; q < links.size(); ++q) {
			NodeLink link = links[q];

			if(link.id.id == destroyed_link_id) {
			    links.erase(links.begin() + q);
			}
		    }
		}

		for(int k = 0; k < node_inputs.size(); ++k) {
		    NodeProperty* prop = &node_inputs[k];
		    std::vector<NodeLink>& links = prop->links;

		    for(int q = 0; q < links.size(); ++q) {
			NodeLink link = links[q];

			if(link.id.id == destroyed_link_id) {
			    links.erase(links.begin() + q);
			}
		    }
		}
	    }
	}
    }
    
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

	// Render tabs
	if(ImGui::BeginTabBar("EditorTabBar")) {
	    if(ImGui::BeginTabItem("Layout")) {
		showViewSpace();
		ImGui::EndTabItem();
	    }
	    if(ImGui::BeginTabItem("Shading")) {
		showNodeSpace();
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
    glm::vec3 cam_pos = main_camera->getPosition();
    float far = main_camera->far;
    for(float x = cam_pos.x - far; x < cam_pos.x + far; x += 0.5f) {
	glm::vec3 start = glm::vec3((int)x, 0, (int)(cam_pos.z - far));
	glm::vec3 end = glm::vec3((int)x, 0, (int)(cam_pos.z + far));
	glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
	if((int)x == 0) {
	    color = glm::vec3(1, 0.4, 0.4);
	}
	Gizmos::drawLine(*main_camera, start, end, color);
    }

    for(float z = cam_pos.z - far; z < cam_pos.z + far; z += 0.5f) {
	glm::vec3 start = glm::vec3((int)(cam_pos.x - far), 0, (int)z);
	glm::vec3 end = glm::vec3((int)(cam_pos.x + far), 0, (int)z);
	glm::vec3 color = glm::vec3(0.4, 0.4, 0.4);
	if((int)z == 0) {
	    color = glm::vec3(0.55, 0.8, 0.9);
	}
	Gizmos::drawLine(*main_camera, start, end, color);
    }
}

void Editor::createNode(NodeType type) {
    if(selected_entity && selected_entity.hasComponent<MeshComponent>()) {
	MeshComponent& mesh = selected_entity.getComponent<MeshComponent>();
	Material* mat = mesh.getActiveMaterial();
    
	if(mat) {
	    Node node;
	    node.create(type);
      
	    mat->nodes.push_back(node);
	}
    }
}
