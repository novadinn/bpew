#include "space_shading.h"

#include "../ecs/components/mesh_component.h"

#include "imgui/imgui.h"
#include "imnodes/imnodes.h"
#include "../core/input.h"

void SpaceShadingData::createNode(Entity selected_entity, NodeType type) {
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

EventReceiver *createSpaceShadingReceiver() {
    EventReceiver *receiver = new EventReceiver();

    receiver->clearEvents();

    receiver->onCreate = onCreateSpaceShading;
    receiver->onDestroy = onDestroySpaceShading;

    receiver->onDrawUIBegin = onDrawUIBeginSpaceShading;
    receiver->onDrawUI = onDrawUISpaceShading;
    receiver->onDrawUIEnd = onDrawUIEndSpaceShading;

    return receiver;
}

void onCreateSpaceShading(EditorContext *ctx) {
    SpaceShadingData *space_data = new SpaceShadingData();
    ctx->space_shading_data = space_data;
}

void onDestroySpaceShading(EditorContext *ctx) {
    SpaceShadingData *space_data = ctx->space_shading_data;

    delete space_data;
}

void onDrawUIBeginSpaceShading(EditorContext *ctx) {
    ImGui::Begin("Shading");
}

void drawNodeCustomAttribute(NodeInput& input) {
    ImGui::PushItemWidth(150);
    switch(input.type) {
    case NodePropertyType::COLOR: {
	float p[4] = { input.value.color_value.x, input.value.color_value.y, input.value.color_value.z, input.value.color_value.w };
	if(ImGui::DragFloat4(input.id.name.c_str(), p, 0.001f))
	    input.value.color_value = glm::vec4(p[0], p[1], p[2], p[3]);
    }
	break;
    case NodePropertyType::VECTOR3: {
	float p[3] = { input.value.vector3_value.x, input.value.vector3_value.y, input.value.vector3_value.z };	
	if(ImGui::DragFloat3(input.id.name.c_str(), p, 0.001f))
	    input.value.vector3_value = glm::vec3(p[0], p[1], p[2]);
    }
	break;
    case NodePropertyType::VECTOR2: {
	float p[2] = { input.value.vector2_value.x, input.value.vector2_value.y };
	if(ImGui::DragFloat2(input.id.name.c_str(), p, 0.001f))
	    input.value.vector2_value = glm::vec2(p[0], p[1]);
    }
	break;
    case NodePropertyType::FLOAT: {
	float p = input.value.float_value;
	if(ImGui::DragFloat(input.id.name.c_str(), &p, 0.001f))
	    input.value.float_value = p;
    }
	break;
    case NodePropertyType::INT: {
	int p = input.value.int_value;
	if(ImGui::DragInt(input.id.name.c_str(), &p))
	    input.value.int_value = p;
    }
	break;
    case NodePropertyType::ENUM:		
	break;
    case NodePropertyType::SHADER:
	break;
    case NodePropertyType::TEXTURE:	
	break;
    default:
	printf("unknown node property type\n");
    }
}

void onDrawUISpaceShading(EditorContext *ctx) {
    SpaceShadingData *space_data = ctx->space_shading_data;
    
    // Draw header
    if (ImGui::Button("Material...")) {
	if(ctx->selected_entity && ctx->selected_entity.hasComponent<MeshComponent>()) {
	    ImGui::OpenPopup("MaterialsMenu");
	} else {
	    // TODO: error message popup
	}
    }
  
    if(ImGui::BeginPopup("MaterialsMenu")) {
	MeshComponent& mesh = ctx->selected_entity.getComponent<MeshComponent>();
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

    // Draw node editor
    ImNodes::BeginNodeEditor();
  
    if(ctx->selected_entity && ctx->selected_entity.hasComponent<MeshComponent>()) {
	MeshComponent& mesh = ctx->selected_entity.getComponent<MeshComponent>();

	Material* mat = mesh.getActiveMaterial();
	if(mat) {
	    for(int i = 0; i < mat->nodes.size(); ++i) {
		Node& node = mat->nodes[i];
		std::vector<NodeOutput>& outputs = node.outputs;
		std::vector<NodeInput>& inputs = node.inputs;

		ImNodes::BeginNode(node.id.id);
	
		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted(node.id.name.c_str());
		ImNodes::EndNodeTitleBar();
	
		// Outputs
		for(int j = 0; j < outputs.size(); ++j) {
		    NodeOutput& prop = outputs[j];
	  
		    ImNodes::BeginOutputAttribute(prop.id.id);
		    ImGui::Text("%s", prop.id.name.c_str());
		    ImNodes::EndOutputAttribute();
		}

		// Inputs
		for(int j = 0; j < inputs.size(); ++j) {
		    NodeInput& prop = inputs[j];

		    if(prop.source == NodePropertySource::ATTR) {
			ImNodes::BeginInputAttribute(prop.id.id);
			ImGui::Text("%s", prop.id.name.c_str());
			ImNodes::EndInputAttribute();
		    } else if(prop.source == NodePropertySource::UNIFORM) {
			drawNodeCustomAttribute(prop);
		    }
		}
	
		ImNodes::EndNode();
	    }	    
	}	
    }    

    // Context menu
    // TODO: not all of those are set right now
    if(ImGui::BeginPopupContextWindow()) {
	if(ImGui::BeginMenu("Add")) {
	    if(ImGui::BeginMenu("Input")) {
		if(ImGui::Button("RGB")) {
		    space_data->createNode(ctx->selected_entity, NodeType::RGB);
		}
	
		ImGui::EndMenu();
	    }
	    if(ImGui::BeginMenu("Output")) {
		if(ImGui::Button("Material Output")) {
		    space_data->createNode(ctx->selected_entity, NodeType::MATERIAL_OUTPUT);
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
    if(ctx->selected_entity && ctx->selected_entity.hasComponent<MeshComponent>()) {
	MeshComponent& mesh = ctx->selected_entity.getComponent<MeshComponent>();
	Material* mat = mesh.getActiveMaterial();
    
	if(mat) {
	    for(int i = 0; i < mat->nodes.size(); ++i) {
		Node& node = mat->nodes[i];
		// Draw Link only from output nodes - dont need to do this twice
		std::vector<NodeOutput>& outputs = node.outputs;

		for(int j = 0; j < outputs.size(); ++j) {
		    NodeOutput& prop = outputs[j];
		    NodeLink* link = prop.link;

		    if(link) {
			int link_id = link->id.id;
			int output_id = link->output->id.id;
			int input_id = link->input->id.id;
		    
			ImNodes::Link(link_id, output_id, input_id);
		    }		    		   		    
		}
	    }
	}
    }
  
    ImNodes::EndNodeEditor();

    // Update links
    int output_id, input_id;
    NodeOutput *output_prop = nullptr;
    NodeInput *input_prop = nullptr;
    uint output_node, input_node;
    if (ImNodes::IsLinkCreated(&output_id, &input_id)) {
	// NOTE: assumes that material and mesh are there
	MeshComponent& mesh = ctx->selected_entity.getComponent<MeshComponent>();
	Material *mat = mesh.getActiveMaterial();
	std::vector<Node>& nodes = mat->nodes;

	// TODO: slow linear search
	for(int i = 0; i < nodes.size(); ++i) {
	    Node& node = nodes[i];
	    std::vector<NodeOutput>& node_outputs = node.outputs;
	    std::vector<NodeInput>& node_inputs = node.inputs;

	    for(int j = 0; j < node_outputs.size(); ++j) {
		NodeOutput* prop = &node_outputs[j];

		if(prop->id.id == output_id) {
		    output_prop = prop;
		    output_node = i;
		}
	    }

	    for(int j = 0; j < node_inputs.size(); ++j) {
		NodeInput* prop = &node_inputs[j];
	
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
	NodeLink* link = new NodeLink();
	link->create(output_prop, input_prop, output_node, input_node);
	
	if(output_prop->link) {
	    output_prop->link->input->link = nullptr;
	    delete output_prop->link;	    
	}
	output_prop->link = link;
	if(input_prop->link) {
	    input_prop->link->output->link = nullptr;
	    delete input_prop->link;	    
	}
	input_prop->link = link;
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
	    MeshComponent& mesh = ctx->selected_entity.getComponent<MeshComponent>();
	    Material *mat = mesh.getActiveMaterial();
	    std::vector<Node>& nodes = mat->nodes;
      
	    for(int j = 0; j < nodes.size(); ++j) {
		Node& node = nodes[j];
		std::vector<NodeOutput>& node_outputs = node.outputs;
		std::vector<NodeInput>& node_inputs = node.inputs;

		for(int k = 0; k < node_outputs.size(); ++k) {
		    NodeOutput* prop = &node_outputs[k];
		    NodeLink* link = prop->link;
		    
		    if(prop->link && link->id.id == destroyed_link_id) {
			prop->link->input->link = nullptr;
			delete prop->link;
			prop->link = nullptr;
			return;
		    }		   
		}

		for(int k = 0; k < node_inputs.size(); ++k) {
		    NodeInput* prop = &node_inputs[k];		    		    
		    
		    if(prop->link && prop->link->id.id == destroyed_link_id) {
			prop->link->output->link = nullptr;
			delete prop->link;
			prop->link = nullptr;
			return;
		    }		    
		}
	    }
	}
    }

}

void onDrawUIEndSpaceShading(EditorContext *ctx) {
    ImGui::End();
}
