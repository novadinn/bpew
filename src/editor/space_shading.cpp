#include "space_shading.h"

#include "../ecs/components/mesh_component.h"

#include "imgui/imgui.h"
#include "imnodes/imnodes.h"
#include "../core/input.h"

void SpaceShading::begin() {
    ImGui::Begin("Shading");
}

void SpaceShading::drawHeader(Entity selected_entity) {
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
}

void SpaceShading::drawNodeEditor(Entity selected_entity) {
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

    drawContextMenu(selected_entity);

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

    updateLinks(selected_entity);
}
    
void SpaceShading::end() {
    ImGui::End();
}

void SpaceShading::drawContextMenu(Entity selected_entity) {
    // TODO: not all of those are set right now
    if(ImGui::BeginPopupContextWindow()) {
	if(ImGui::BeginMenu("Add")) {
	    if(ImGui::BeginMenu("Input")) {
		if(ImGui::Button("RGB")) {
		    createNode(selected_entity, NodeType::RGB);
		}
	
		ImGui::EndMenu();
	    }
	    if(ImGui::BeginMenu("Output")) {
		if(ImGui::Button("Material Output")) {
		    createNode(selected_entity, NodeType::MATERIAL_OUTPUT);
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
}

void SpaceShading::updateLinks(Entity selected_entity) {
    int output_id, input_id;
    NodeProperty *output_prop = nullptr, *input_prop = nullptr;
    uint output_node, input_node;
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
		    output_node = i;
		}
	    }

	    for(int j = 0; j < node_inputs.size(); ++j) {
		NodeProperty* prop = &node_inputs[j];
	
		if(prop->id.id == input_id) {
		    input_prop = prop;
		    input_node = i;
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
	link.create(output_prop, input_prop, output_node, input_node);
    
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

}

void SpaceShading::createNode(Entity selected_entity, NodeType type) {
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
