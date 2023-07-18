#include "space_shading.h"

#include "../core/input.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "imgui/imgui.h"
#include "imnodes/imnodes.h"

void SpaceShadingData::createNode(Material *mat, NodeType type) {
  Node node;
  node.create(type);

  mat->nodes.push_back(node);
}

Material *SpaceShadingData::activeMaterial() {
  return MaterialManager::getMaterial(active_material_index);
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

void drawNodeRGB(SpaceShadingData *ctx, Node &node) {
  NodeInput &color_input = node.inputs[0];

  drawNodeInputColor3Picker(ctx, color_input, "Color");

  drawNodeOutputAttributes(ctx, node.outputs);
}

void drawNodeMaterialOutput(SpaceShadingData *ctx, Node &node) {
  drawNodeInputAttributes(ctx, node.inputs);
}

void drawNodeImageTexture(SpaceShadingData *ctx, Node &node) {
  NodeInput &texture_input = node.inputs[0];

  ImGui::Text("Image path");
  ImGui::SameLine();
  if (ImGui::Button("...")) {
    ImGuiFileDialog::Instance()->OpenDialog("LoadTexture", "Choose File",
                                            ".png,.jpg", ".");
  }

  if (ImGuiFileDialog::Instance()->Display("LoadTexture")) {
    if (ImGuiFileDialog::Instance()->IsOk()) {
      std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string name = ImGuiFileDialog::Instance()->GetCurrentFileName();

      texture_input.value.texture_value.createFromFile(path.c_str());
    }

    ImGuiFileDialog::Instance()->Close();
  }

  drawNodeInputAttributes(ctx, node.inputs);

  drawNodeOutputAttributes(ctx, node.outputs);
}

void drawNodeTextureCoordinate(SpaceShadingData *ctx, Node &node) {
  drawNodeInputAttributes(ctx, node.inputs);
  drawNodeOutputAttributes(ctx, node.outputs);
}

void drawNodePrincipledBSDF(SpaceShadingData *ctx, Node &node) {
  NodeInput &base_color_input = node.inputs[0];
  NodeInput &alpha_input = node.inputs[1];

  drawNodeOutputAttributes(ctx, node.outputs);

  drawNodeInputAttribute(ctx, base_color_input);
  drawNodeInputColor3Edit(ctx, base_color_input, "Base Color");
  drawNodeInputAttribute(ctx, alpha_input);
  drawNodeInputFloatDrag(ctx, alpha_input, "Alpha", 0.0f, 1.0f);
}

NodePropertyType mixType(int value) {
  NodePropertyType mix_type;
  switch (value) {
  case 0:
    mix_type = NodePropertyType::FLOAT;
    break;
  case 1:
    mix_type = NodePropertyType::VECTOR3;
    break;
  case 2:
    mix_type = NodePropertyType::COLOR;
    break;
  default:
    mix_type = NodePropertyType::FLOAT;
  }
  return mix_type;
}

void drawNodeMix(SpaceShadingData *ctx, Node &node) {
  NodeInput &data_type_input = node.inputs[0];
  NodeInput &factor_input = node.inputs[1];

  static const char *values[3] = {"Float", "Vector", "Color"};
  int prev_type = data_type_input.value.enum_value;
  drawNodeInputSelect(ctx, data_type_input, "Data Type", values, 3);

  NodePropertyType mix_type = mixType(data_type_input.value.enum_value);
  if (data_type_input.value.enum_value != prev_type) {

    for (int i = 2; i < node.inputs.size(); ++i) {
      NodeInput &input = node.inputs[i];

      input.enabled = input.type == mix_type;
    }
  }

  for (int i = 0; i < node.outputs.size(); ++i) {
    NodeOutput &output = node.outputs[i];

    output.enabled = output.type == mix_type;
  }
  drawNodeOutputAttribute(ctx, node.outputs[data_type_input.value.enum_value]);

  drawNodeInputAttribute(ctx, factor_input);
  drawNodeInputFloatDrag(ctx, factor_input, "Factor", 0.0f, 1.0f);

  for (int i = 2; i < node.inputs.size(); ++i) {
    NodeInput &input = node.inputs[i];

    if (mix_type != input.type)
      continue;

    drawNodeInputAttribute(ctx, input);
    switch (input.type) {
    case NodePropertyType::FLOAT:
      drawNodeInputFloatDrag(ctx, input, input.id.name.c_str(), FLT_MIN,
                             FLT_MAX);
      break;
    case NodePropertyType::VECTOR3:
      drawNodeInputFloatDrag3(ctx, input, input.id.name.c_str(), FLT_MIN,
                              FLT_MAX);
      break;
    case NodePropertyType::COLOR:
      drawNodeInputColor3Edit(ctx, input, input.id.name.c_str());
      break;
    }
  }
}

void drawNodeInputSelect(SpaceShadingData *ctx, NodeInput &input,
                         const char *title, const char **names, uint size) {
  if (!input.useUniform(ctx->activeMaterial()))
    return;

  if (ImGui::Button(names[input.value.enum_value]))
    ImGui::OpenPopup(title);

  if (ImGui::BeginPopup(title)) {
    for (int i = 0; i < size; ++i) {
      if (ImGui::Selectable(names[i]))
        input.value.enum_value = i;
    }

    ImGui::EndPopup();
  }
}

void drawNodeInputFloatDrag(SpaceShadingData *ctx, NodeInput &input,
                            const char *title, float min, float max) {
  if (!input.useUniform(ctx->activeMaterial()))
    return;
  ImGui::DragFloat(title, &input.value.float_value, 0.001f, min, max);
}

void drawNodeInputFloatDrag3(SpaceShadingData *ctx, NodeInput &input,
                             const char *title, float min, float max) {
  if (!input.useUniform(ctx->activeMaterial()))
    return;

  float values[3] = {
      input.value.vector3_value.x,
      input.value.vector3_value.y,
      input.value.vector3_value.z,
  };

  if (ImGui::DragFloat3(title, values, min, max)) {
    input.value.vector3_value.x = values[0];
    input.value.vector3_value.y = values[1];
    input.value.vector3_value.z = values[2];
  }
}

void drawNodeInputColor3Picker(SpaceShadingData *ctx, NodeInput &input,
                               const char *title, ImGuiColorEditFlags flags) {
  if (!input.useUniform(ctx->activeMaterial()))
    return;
  static ImGuiColorEditFlags color_flags =
      ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoAlpha | flags;

  float color[3] = {input.value.vector3_value.x, input.value.vector3_value.y,
                    input.value.vector3_value.z};

  if (ImGui::ColorPicker3(title, color, color_flags)) {
    input.value.vector3_value.x = color[0];
    input.value.vector3_value.y = color[1];
    input.value.vector3_value.z = color[2];
  }
}

void drawNodeInputColor3Edit(SpaceShadingData *ctx, NodeInput &input,
                             const char *title, ImGuiColorEditFlags flags) {
  if (!input.useUniform(ctx->activeMaterial()))
    return;
  static ImGuiColorEditFlags color_flags =
      ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoAlpha |
      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | flags;

  float color[3] = {input.value.vector3_value.x, input.value.vector3_value.y,
                    input.value.vector3_value.z};

  std::string name = std::string(title) + "##" + std::to_string(input.id.id);
  if (ImGui::ColorEdit3(name.c_str(), color, color_flags)) {
    input.value.vector3_value.x = color[0];
    input.value.vector3_value.y = color[1];
    input.value.vector3_value.z = color[2];
  }
}

void drawNode(SpaceShadingData *ctx, Node &node) {
  ImGui::PushItemWidth(150);
  ImGui::PushID(node.id.id);

  ImNodes::BeginNode(node.id.id);

  ImNodes::BeginNodeTitleBar();
  ImGui::TextUnformatted(node.id.name.c_str());
  ImNodes::EndNodeTitleBar();

  switch (node.type) {
  case NodeType::RGB:
    drawNodeRGB(ctx, node);
    break;
  case NodeType::TEXTURE_COORDINATE:
    drawNodeTextureCoordinate(ctx, node);
    break;
  case NodeType::MATERIAL_OUTPUT:
    drawNodeMaterialOutput(ctx, node);
    break;
  case NodeType::PRINCIPLED_BSDF:
    drawNodePrincipledBSDF(ctx, node);
    break;
  case NodeType::IMAGE_TEXTURE:
    drawNodeImageTexture(ctx, node);
    break;
  case NodeType::MIX:
    drawNodeMix(ctx, node);
    break;
  default:
    printf("unknown node type\n");
  }

  ImNodes::EndNode();

  ImGui::PopID();
}

void drawNodeInputAttribute(SpaceShadingData *ctx, NodeInput &input) {
  if (input.source == NodePropertySource::OUTPUT ||
      input.source == NodePropertySource::OUTPUT_UNIFORM) {
    ImNodes::BeginInputAttribute(input.id.id);
    ImGui::Text("%s", input.id.name.c_str());
    ImNodes::EndInputAttribute();
  }
}

void drawNodeOutputAttribute(SpaceShadingData *ctx, NodeOutput &output) {
  ImNodes::BeginOutputAttribute(output.id.id);
  ImGui::Text("%s", output.id.name.c_str());
  ImNodes::EndOutputAttribute();
}

void drawNodeInputAttributes(SpaceShadingData *ctx,
                             std::vector<NodeInput> &inputs) {
  for (NodeInput &input : inputs) {
    if (input.enabled)
      drawNodeInputAttribute(ctx, input);
  }
}
void drawNodeOutputAttributes(SpaceShadingData *ctx,
                              std::vector<NodeOutput> &outputs) {
  for (NodeOutput &output : outputs) {
    if (output.enabled)
      drawNodeOutputAttribute(ctx, output);
  }
}

void onDrawUISpaceShading(EditorContext *ctx) {
    SpaceShadingData *space_data = ctx->space_shading_data;
    Material *mat =
        MaterialManager::getMaterial(space_data->active_material_index);

    // Draw header
    if (ImGui::Button("Material...")) {
      ImGui::OpenPopup("MaterialsMenu");
    }

    if (ImGui::BeginPopup("MaterialsMenu")) {
      std::vector<Material> &materials = MaterialManager::materials;

      static char material_name[255];
      if (ImGui::InputText("New Material", material_name, 255,
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
        std::string name(material_name);
        MaterialManager::addMaterial(name);

        memset(material_name, '\0', sizeof(material_name));
      }
      if (ImGui::Button("Destroy Material")) {
        if (MaterialManager::validMaterialIndex(
                space_data->active_material_index)) {
          MaterialManager::removeMaterial(space_data->active_material_index);

          if (space_data->active_material_index == materials.size()) {
            --space_data->active_material_index;
          }
        }
      }

      if (ImGui::CollapsingHeader("Materials")) {
        for (int i = 0; i < materials.size(); ++i) {
          Material &mat = materials[i];

          if (ImGui::Selectable(mat.name.c_str(),
                                space_data->active_material_index == i,
                                ImGuiSelectableFlags_DontClosePopups)) {
            space_data->active_material_index = i;
          }
        }
      }

      ImGui::EndPopup();
    }

    // Draw node editor
    ImNodes::BeginNodeEditor();

    if (mat) {
      for (int i = 0; i < mat->nodes.size(); ++i) {
        Node &node = mat->nodes[i];

        drawNode(space_data, node);
      }
    }

    // Context menu
    // TODO: not all of those are set right now
    if(ImGui::BeginPopupContextWindow()) {
			if(ImGui::BeginMenu("Add")) {
				if(ImGui::BeginMenu("Input")) {
					if (ImGui::Button("RGB")) {
						space_data->createNode(mat, NodeType::RGB);
					}
					if (ImGui::Button("Texture Coordinate")) {
						space_data->createNode(mat, NodeType::TEXTURE_COORDINATE);
					}

					ImGui::EndMenu();
				}
				if(ImGui::BeginMenu("Output")) {
					if(ImGui::Button("Material Output")) {
						space_data->createNode(mat, NodeType::MATERIAL_OUTPUT);
					}
	
					ImGui::EndMenu();
				}
				if(ImGui::BeginMenu("Shader")) {
					if (ImGui::Button("Principled BSDF")) {
						space_data->createNode(mat, NodeType::PRINCIPLED_BSDF);
					}

					ImGui::EndMenu();
				}
				if(ImGui::BeginMenu("Texture")) {
					if (ImGui::Button("Image Texture")) {
						space_data->createNode(mat, NodeType::IMAGE_TEXTURE);
					}

					ImGui::EndMenu();
				}
				if(ImGui::BeginMenu("Color")) {
	
					ImGui::EndMenu();
				}
				if(ImGui::BeginMenu("Vector")) {
	
					ImGui::EndMenu();
				}
				if(ImGui::BeginMenu("Converter")) {
					if (ImGui::Button("Mix")) {
						space_data->createNode(mat, NodeType::MIX);
					}

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
    if (mat) {
      for (int i = 0; i < mat->nodes.size(); ++i) {
        Node &node = mat->nodes[i];
        // Draw Link only from output nodes - dont need to do this twice
        std::vector<NodeOutput> &outputs = node.outputs;

        for (int j = 0; j < outputs.size(); ++j) {
          NodeOutput &prop = outputs[j];
          if (!prop.enabled)
            continue;

          for (auto &link : prop.links) {
            if (!link.input(mat)->enabled)
              continue;
            int link_id = link.id.id;
            int output_id = link.output(mat)->id.id;
            int input_id = link.input(mat)->id.id;

            ImNodes::Link(link_id, output_id, input_id);
          }
        }
      }
    }
    ImNodes::EndNodeEditor();

    // Update links
    int output_id, input_id;
    uint output_node, input_node;
    int output_prop = -1, input_prop = -1;
    if (ImNodes::IsLinkCreated(&output_id, &input_id)) {
      // NOTE: assumes that material and mesh are there
      std::vector<Node> &nodes = mat->nodes;

      // TODO: slow linear search
      for (int i = 0; i < nodes.size(); ++i) {
        Node &node = nodes[i];
        std::vector<NodeOutput> &node_outputs = node.outputs;
        std::vector<NodeInput> &node_inputs = node.inputs;

        for (int j = 0; j < node_outputs.size(); ++j) {
          NodeOutput *prop = &node_outputs[j];

          if (prop->id.id == output_id) {
            output_prop = j;
            output_node = i;
          }
        }

        for (int j = 0; j < node_inputs.size(); ++j) {
          NodeInput *prop = &node_inputs[j];

          if (prop->id.id == input_id) {
            input_prop = j;
            input_node = i;
          }
        }

        if (output_prop != -1 && input_prop != -1) {
          break;
        }
        }
    }

    if (output_prop != -1 && input_prop != -1) {
      // TODO: should we add this to both input and output node or not?
      NodeLink *link = new NodeLink();
      link->create(output_prop, input_prop, output_node, input_node);

      NodeOutput *output = link->output(mat);
      NodeInput *input = link->input(mat);

      output->links.push_back(*link);
      if (input->link) {
        NodeOutput *prev_output = input->link->output(mat);
        for (int i = 0; i < prev_output->links.size(); ++i) {
          NodeLink &l = prev_output->links[i];

          if (l.id.id == input->link->id.id) {
            prev_output->links.erase(prev_output->links.begin() + i);
            break;
          }
        }
        delete input->link;
      }
      input->link = link;
    }

    // TODO: ImNodes::IsLinkDestroyed is not impelemted on the imnodes side (or thats just me stupid),
    // so check for deletion by our own
    int num_selected = ImNodes::NumSelectedLinks();
    // TODO: dont check for input in here - use update method
    if(num_selected > 0 && Input::wasKeyReleased(SDLK_DELETE)) {
      int *selected_links = new int[num_selected];
      ImNodes::GetSelectedLinks(&selected_links[0]);

      // TODO: slow linear search
      for (int i = 0; i < num_selected; ++i) {
        int destroyed_link_id = selected_links[i];
        std::vector<Node> &nodes = mat->nodes;

        for (int j = 0; j < nodes.size(); ++j) {
          Node &node = nodes[j];
          std::vector<NodeOutput> &node_outputs = node.outputs;
          std::vector<NodeInput> &node_inputs = node.inputs;

          for (int k = 0; k < node_outputs.size(); ++k) {
            NodeOutput *prop = &node_outputs[k];
            std::vector<NodeLink> &links = prop->links;

            for (int m = 0; m < links.size(); ++m) {
              NodeLink &link = links[m];

              if (link.id.id == destroyed_link_id) {
                NodeInput *input = link.input(mat);
                delete input->link;
                input->link = nullptr;
                links.erase(links.begin() + m);
                return;
              }
            }
          }
        }
        }
    }

}

void onDrawUIEndSpaceShading(EditorContext *ctx) {
    ImGui::End();
}
