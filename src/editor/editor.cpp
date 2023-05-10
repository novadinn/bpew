#include "editor.h"

#include "../ecs/components.h"
#include "../graphics/model.h"
#include "../graphics/renderer.h"
#include "../core/input.h"

#include "imgui/imgui.h"

const char* shader_vs = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 worldPosition = model* vec4(aPos, 1.0f);

    vs_out.fragPos = vec3(worldPosition);
    vs_out.normal = mat3(transpose(inverse(model))) * aNormal;

	gl_Position = projection * view * worldPosition;
}
)";
const char* shader_fs = R"(
#version 460 core

out vec4 fragColor;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
} vs_in;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform float shininess;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main() {
    vec3 norm = normalize(vs_in.normal);
    vec3 viewDir = normalize(viewPos - vs_in.fragPos);

    vec3 result = calcDirLight(dirLight, norm, viewDir);

	fragColor = vec4(0.5f, 0.5f, 0.5f, 1.0f) + vec4(result, 0.0f);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    
    return (ambient + diffuse + specular);
}
)";

void Editor::create() {
	Model model;
	model.loadFromPath("models/monkey/monkey.obj");
	Shader model_shader;
	model_shader.createFromSource(shader_vs, shader_fs);
	
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
	mesh.shader = model_shader;
	
	Entity object2 = scene->createEntity("Monkey2");
	auto& mesh2 = object2.addComponent<MeshComponent>();
	mesh2.model = model;
	mesh2.shader = model_shader;
	auto& tr = object.getComponent<TransformComponent>();
	tr.position = glm::vec3(3.0, 0, 0);
	tr.scale = glm::vec3(0.5, 0.5, 0.5);
	tr.rotation = glm::vec3(36, 80, 170);

	FramebufferData data;
	data.width = 1;
	data.height = 1;
	
	framebuffer.create(data);
}

void Editor::destroy() {
	delete scene;
}

void Editor::onUpdate() {

	float delta_time = 0.01f;
	
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
	if(Input::wasWheelMoved()) {
		main_camera->zoom(delta_time * wheel_movement.y);
	}
}

void Editor::onDraw() {
	scene->onResize(viewport_size.x, viewport_size.y);

	FramebufferData data = framebuffer.getFramebufferData();
	if(viewport_size.x > 0.0f && viewport_size.y > 0.0f &&
	   (data.width != viewport_size.x || data.height != viewport_size.y)) {
		
	    framebuffer.resize(viewport_size.x, viewport_size.y);
	}

	framebuffer.bind();
	Renderer::clear();
	scene->onDraw();
	framebuffer.unbind();

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
	
	showViewport();
	showMenuBar();
	showHierarchyPanel();
	showInspectorPanel();

	// NOTE: dockspace end
	ImGui::End();
}

void Editor::showViewport() {
	ImGui::Begin("Scene");
	ImVec2 vpsize = ImGui::GetContentRegionAvail();
	viewport_size = glm::vec2(vpsize.x, vpsize.y);
	
	uint fbid = framebuffer.getColorAttachmentID();
	ImGui::Image(reinterpret_cast<void*>(fbid), ImVec2{ viewport_size.x, viewport_size.y },
				 ImVec2{ 0, 1 }, ImVec2{ 1, 0 });	
	
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
			}
		}

		if(ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");
	    if(ImGui::BeginPopup("AddComponent")) {
			showAddComponentPopup<TransformComponent>("Transform");
			showAddComponentPopup<CameraComponent>("Camera");
			showAddComponentPopup<MeshComponent>("Mesh");
		
			ImGui::EndPopup();
	    }

	    if(ImGui::Button("Remove Component"))
			ImGui::OpenPopup("RemoveComponent");
	    if(ImGui::BeginPopup("RemoveComponent")) {
			showRemoveComponentPopup<TransformComponent>("Transform");
			showRemoveComponentPopup<CameraComponent>("Camera");
			showRemoveComponentPopup<MeshComponent>("Mesh");
		
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
