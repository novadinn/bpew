#include "space_shading.h"

#include "../core/input.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "imgui/imgui.h"
#include "imnodes/imnodes.h"

void SpaceShadingData::createNode(Material *mat, NodeType type) {	
	if (mat) {
		Node node;
		node.create(type);
		
		mat->nodes.push_back(node);
	}
}

Material *SpaceShadingData::activeMaterial() {
  return MaterialManager::getMaterial(active_material_index);
}

EventReceiver *createSpaceShadingReceiver() {
  EventReceiver *receiver = new EventReceiver();

  receiver->clearEvents();

  receiver->onCreate = onCreateSpaceShading;
  receiver->onDestroy = onDestroySpaceShading;

  receiver->onDrawUI = onDrawUISpaceShading;

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

void onDrawUIBeginSpaceShading(EditorContext *ctx) { ImGui::Begin("Shading"); }

void drawNodeRGB(SpaceShadingData *ctx, Node &node) {
  NodeInput &color_input = node.inputs[0];

  drawNodeInputColor3Picker(color_input, "Color");

  drawNodeOutputAttributes(ctx, node.outputs);
}

void drawNodeBevel(SpaceShadingData *ctx, Node &node) {
  NodeInput &radius_input = node.inputs[0];
  NodeInput &normal_input = node.inputs[1];

  drawNodeOutputAttribute(ctx, node.outputs[0]);

  if(drawNodeInputBegin(ctx, radius_input)) {
		drawNodeInputFloatDrag(radius_input, "Radius", 0.0f, 1000.0f);

		drawNodeInputEnd();
	}
	
	if (drawNodeInputBegin(ctx, normal_input)) {
		drawNodeInputEnd();
	}
}

void drawNodeMaterialOutput(SpaceShadingData *ctx, Node &node) {
  drawNodeInputAttributes(ctx, node.inputs);
}

void drawNodeBrickTexture(SpaceShadingData *ctx, Node &node) {
	NodeInput &vector_input = node.inputs[0];
	NodeInput &color1_input = node.inputs[1];
	NodeInput &color2_input = node.inputs[2];
	NodeInput &mortar_input = node.inputs[3];
	NodeInput &scale_input = node.inputs[4];
	NodeInput &mortar_size_input = node.inputs[5];
	NodeInput &mortar_smooth_input = node.inputs[6];
	NodeInput &bias_input = node.inputs[7];
	NodeInput &brick_width_input = node.inputs[8];
	NodeInput &row_height_input = node.inputs[9];
	NodeInput &offset_amount_input = node.inputs[10];
	NodeInput &offset_frequency_input = node.inputs[11];
	NodeInput &squash_amount_input = node.inputs[12];
	NodeInput &squash_frequency_input = node.inputs[13];

	drawNodeOutputAttributes(ctx, node.outputs);
	
	drawNodeInputFloatDrag(offset_amount_input, "Offset", 0.0f, 1.0f);
	drawNodeInputIntSlider(offset_frequency_input, "Offset Frequency", 1, 99);

	drawNodeInputFloatDrag(squash_amount_input, "Squash", 0.0f, 99.0f);
	drawNodeInputIntSlider(squash_frequency_input, "Squash Frequency", 1, 99);

	drawNodeInputBegin(ctx, vector_input);

	if (drawNodeInputBegin(ctx, color1_input)) {
		drawNodeInputColor3Edit(color1_input, "Color1");

		drawNodeInputEnd();
	}

	if (drawNodeInputBegin(ctx, color2_input)) {
		drawNodeInputColor3Edit(color2_input, "Color2");

		drawNodeInputEnd();
	}

	if (drawNodeInputBegin(ctx, mortar_input)) {
		drawNodeInputColor3Edit(mortar_input, "Mortar");

		drawNodeInputEnd();
	}

	if (drawNodeInputBegin(ctx, scale_input)) {
		drawNodeInputFloatDrag(scale_input, "Scale", -1000.0f, 1000.0f);

		drawNodeInputEnd();
	}

	if (drawNodeInputBegin(ctx, mortar_size_input)) {
		drawNodeInputFloatDrag(mortar_size_input, "Mortar Size", 0.0f, 0.125f);

		drawNodeInputEnd();
	}

	if (drawNodeInputBegin(ctx, mortar_smooth_input)) {
		drawNodeInputFloatDrag(mortar_smooth_input, "Mortar Smooth", 0.0f, 1.0f);

		drawNodeInputEnd();
	}

	if (drawNodeInputBegin(ctx, bias_input)) {
		drawNodeInputFloatDrag(bias_input, "Bias", -1.0f, 1.0f);

		drawNodeInputEnd();
	}

	if (drawNodeInputBegin(ctx, brick_width_input)) {
		drawNodeInputFloatDrag(brick_width_input, "Brick Width", 0.01f, 100.0f);

		drawNodeInputEnd();
	}

	if (drawNodeInputBegin(ctx, row_height_input)) {
		drawNodeInputFloatDrag(row_height_input, "Row Height", 0.01f, 100.0f);

		drawNodeInputEnd();
	}
}

void drawNodeCheckerTexture(SpaceShadingData *ctx, Node &node) {
	NodeInput &vector_input = node.inputs[0];
	NodeInput &color1_input = node.inputs[1];
	NodeInput &color2_input = node.inputs[2];
	NodeInput &scale_input = node.inputs[3];

	drawNodeOutputAttributes(ctx, node.outputs);

	drawNodeInputBegin(ctx, vector_input);

	if (drawNodeInputBegin(ctx, color1_input)) {
		drawNodeInputColor3Edit(color1_input, "Color1");
		
		drawNodeInputEnd();
	}

	if (drawNodeInputBegin(ctx, color2_input)) {
		drawNodeInputColor3Edit(color2_input, "Color2");
		
		drawNodeInputEnd();
	}

	if (drawNodeInputBegin(ctx, scale_input)) {
		drawNodeInputFloatDrag(scale_input, "Scale", FLT_MIN, FLT_MAX);
		
		drawNodeInputEnd();
	}
}

void drawNodeImageTexture(SpaceShadingData *ctx, Node &node) {
  NodeInput &texture_input = node.inputs[0];

  if (ImGui::Button("Select"))
    ImGui::OpenPopup("Select Texture");

  if (ImGui::BeginPopup("Select Texture")) {
    std::vector<Texture2D> &textures = Texture2D::textures;

    for (int i = 0; i < textures.size(); ++i) {
      const bool selected = texture_input.value.texture_value == i;
      Texture2D *texture = Texture2D::getTexture(i);
      if (ImGui::Selectable(texture->name.c_str()))
        texture_input.value.texture_value = i;

      if (selected)
        ImGui::SetItemDefaultFocus();
    }

    ImGui::EndPopup();
  }

  ImGui::SameLine();

  std::string name = "";
  Texture2D *texture = Texture2D::getTexture(texture_input.value.texture_value);
  if (texture) {
    name = texture->name;
  }

  ImGui::Text(name.c_str());

  ImGui::SameLine();
  if (ImGui::Button("...")) {
    ImGuiFileDialog::Instance()->OpenDialog("LoadTexture", "Choose File",
                                            ".png,.jpg", ".");
  }

  if (ImGuiFileDialog::Instance()->Display("LoadTexture")) {
    if (ImGuiFileDialog::Instance()->IsOk()) {
      std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string name = ImGuiFileDialog::Instance()->GetCurrentFileName();

      uint index = Texture2D::textures.size();
      Texture2D tex;
      tex.name = name;
      tex.createFromFile(path.c_str());

      texture_input.value.texture_value = index;
    }

    ImGuiFileDialog::Instance()->Close();
  }

  drawNodeInputAttributes(ctx, node.inputs);

  drawNodeOutputAttributes(ctx, node.outputs);
}

void drawNodeBrightnessContrast(SpaceShadingData *ctx, Node &node) {
  NodeInput &color_input = node.inputs[0];
  NodeInput &brightness_input = node.inputs[1];
  NodeInput &contrast_input = node.inputs[2];

  drawNodeOutputAttribute(ctx, node.outputs[0]);
	
  if (drawNodeInputBegin(ctx, color_input)) {
		drawNodeInputColor3Edit(color_input, "Color");

		drawNodeInputEnd();
	}

  if (drawNodeInputBegin(ctx, brightness_input)) {
		drawNodeInputFloatDrag(brightness_input, "Bright", -100.0f, 100.0f);

		drawNodeInputEnd();
	}
	
  if (drawNodeInputBegin(ctx, contrast_input)) {
		drawNodeInputFloatDrag(contrast_input, "Contrast", -100.0f, 100.0f);

		drawNodeInputEnd();
	}
}

void drawNodeGamma(SpaceShadingData *ctx, Node &node) {
  NodeInput &color_input = node.inputs[0];
  NodeInput &gamma_input = node.inputs[1];

  drawNodeOutputAttribute(ctx, node.outputs[0]);

  if (drawNodeInputBegin(ctx, color_input)) {
		drawNodeInputColor3Edit(color_input, "Color");

		drawNodeInputEnd();
	}

  if (drawNodeInputBegin(ctx, gamma_input)) {
		drawNodeInputFloatDrag(gamma_input, "Gamma", 0.001f, 10.0f);
		
		drawNodeInputEnd();		
	}
}

void drawNodeInvert(SpaceShadingData *ctx, Node &node) {
  NodeInput &fac_input = node.inputs[0];
  NodeInput &color_input = node.inputs[1];

  drawNodeOutputAttribute(ctx, node.outputs[0]);

  if (drawNodeInputBegin(ctx, fac_input)) {
		drawNodeInputFloatDrag(fac_input, "Fac", 0.0f, 1.0f);

		drawNodeInputEnd();
	}

  if (drawNodeInputBegin(ctx, color_input)) {
		drawNodeInputColor3Edit(color_input, "Color");

		drawNodeInputEnd();
	}
}

void drawNodeLightFalloff(SpaceShadingData *ctx, Node &node) {
	NodeInput &strength_input = node.inputs[0];
	NodeInput &smooth_input = node.inputs[1];

	drawNodeOutputAttributes(ctx, node.outputs);

	if (drawNodeInputBegin(ctx, strength_input)) {
		drawNodeInputFloatDrag(strength_input, "Strength", 0.0f, FLT_MAX);

		drawNodeInputEnd();
	}

	if (drawNodeInputBegin(ctx, smooth_input)) {
		drawNodeInputFloatDrag(smooth_input, "Smooth", 0.0f, 1000.0f);

		drawNodeInputEnd();
	}
}

void drawNodeTextureCoordinate(SpaceShadingData *ctx, Node &node) {
  drawNodeInputAttributes(ctx, node.inputs);
  drawNodeOutputAttributes(ctx, node.outputs);
}

void drawNodePrincipledBSDF(SpaceShadingData *ctx, Node &node) {
  NodeInput &base_color_input = node.inputs[0];
  NodeInput &alpha_input = node.inputs[1];

  drawNodeOutputAttributes(ctx, node.outputs);
	
  if (drawNodeInputBegin(ctx, base_color_input)) {
		drawNodeInputColor3Edit(base_color_input, "Base Color");

		drawNodeInputEnd();
	}
	
  if (drawNodeInputBegin(ctx, alpha_input)) {
		drawNodeInputFloatDrag(alpha_input, "Alpha", 0.0f, 1.0f);
		
		drawNodeInputEnd();
	}
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
    mix_type = NodePropertyType::VECTOR4;
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
  drawNodeInputSelect(data_type_input, "Data Type", values, 3);

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

  if (drawNodeInputBegin(ctx, factor_input)) {
		drawNodeInputFloatDrag(factor_input, "Factor", 0.0f, 1.0f);

		drawNodeInputEnd();
	}

  for (int i = 2; i < node.inputs.size(); ++i) {
    NodeInput &input = node.inputs[i];

    if (mix_type != input.type)
      continue;

    if (drawNodeInputBegin(ctx, input)) {
			switch (input.type) {
			case NodePropertyType::FLOAT:
				drawNodeInputFloatDrag(input, input.id.name.c_str(), FLT_MIN,
															 FLT_MAX);
				break;
			case NodePropertyType::VECTOR3:
				drawNodeInputFloatDrag3(input, input.id.name.c_str(), FLT_MIN,
																FLT_MAX);
				break;
			case NodePropertyType::VECTOR4:
				drawNodeInputColor3Edit(input, input.id.name.c_str());
				break;
			}

			drawNodeInputEnd();
		}
  }
}

void drawNodeInputSelect(NodeInput &input,
                         const char *title, const char **names, uint size) {  
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

void drawNodeInputFloatDrag(NodeInput &input,
                            const char *title, float min, float max) {  
  ImGui::DragFloat(title, &input.value.float_value, 0.001f, min, max);
}

void drawNodeInputIntSlider(NodeInput &input,
														const char *title, int min,
														int max) {
	ImGui::SliderInt(title, &input.value.int_value, min, max);
}

void drawNodeInputFloatDrag3(NodeInput &input,
                             const char *title, float min, float max) {  
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

void drawNodeInputColor3Picker(NodeInput &input,
                               const char *title, ImGuiColorEditFlags flags) {  
  ImGuiColorEditFlags color_flags =
      ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoAlpha | flags;

  float color[3] = {input.value.vector4_value.x, input.value.vector4_value.y,
                    input.value.vector4_value.z};

  if (ImGui::ColorPicker3(title, color, color_flags)) {
    input.value.vector4_value.x = color[0];
    input.value.vector4_value.y = color[1];
    input.value.vector4_value.z = color[2];
  }
}

void drawNodeInputColor3Edit(NodeInput &input,
                             const char *title, ImGuiColorEditFlags flags) {  
  ImGuiColorEditFlags color_flags =
      ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoAlpha |
      ImGuiColorEditFlags_NoInputs | flags;

  float color[3] = {input.value.vector4_value.x, input.value.vector4_value.y,
                    input.value.vector4_value.z};
  
  if (ImGui::ColorEdit3(title, color, color_flags)) {
    input.value.vector4_value.x = color[0];
    input.value.vector4_value.y = color[1];
    input.value.vector4_value.z = color[2];
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
  case NodeType::BEVEL:
    drawNodeBevel(ctx, node);
    break;
  case NodeType::MATERIAL_OUTPUT:
    drawNodeMaterialOutput(ctx, node);
    break;
  case NodeType::PRINCIPLED_BSDF:
    drawNodePrincipledBSDF(ctx, node);
    break;
	case NodeType::BRICK_TEXTURE:
		drawNodeBrickTexture(ctx, node);
		break;
	case NodeType::CHECKER_TEXTURE:
		drawNodeCheckerTexture(ctx, node);
		break;
  case NodeType::IMAGE_TEXTURE:
    drawNodeImageTexture(ctx, node);
    break;
  case NodeType::BRIGHTNESS_CONTRAST:
    drawNodeBrightnessContrast(ctx, node);
    break;
  case NodeType::GAMMA:
    drawNodeGamma(ctx, node);
    break;
  case NodeType::INVERT:
    drawNodeInvert(ctx, node);
    break;
	case NodeType::LIGHT_FALLOFF:
		drawNodeLightFalloff(ctx, node);
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

void drawNodeInputEnd() {	
	ImNodes::EndInputAttribute();
}

bool drawNodeInputBegin(SpaceShadingData *ctx, NodeInput &input) {
	if (!input.enabled)
		return false;	

	if (input.source == NodePropertySource::OUTPUT_UNIFORM && !input.link) {
		ImNodes::BeginInputAttribute(input.id.id);
		return true;
	} else if (input.source == NodePropertySource::OUTPUT
			|| (input.source == NodePropertySource::OUTPUT_UNIFORM && input.link)) {
		ImNodes::BeginInputAttribute(input.id.id);
		ImGui::Text(input.id.name.c_str());
		drawNodeInputEnd();
		return false;
	} else {
		return false;
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
		if (drawNodeInputBegin(ctx, input)) {			
			drawNodeInputEnd();
		}
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

	ImGui::Begin("Shading");
	
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
				mat = nullptr;
				
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
  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::BeginMenu("Add")) {
      if (ImGui::BeginMenu("Input")) {
        if (ImGui::Button("Bevel")) {
          space_data->createNode(mat, NodeType::BEVEL);
        }
        if (ImGui::Button("RGB")) {
          space_data->createNode(mat, NodeType::RGB);
        }
        if (ImGui::Button("Texture Coordinate")) {
          space_data->createNode(mat, NodeType::TEXTURE_COORDINATE);
        }

        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Output")) {
        if (ImGui::Button("Material Output")) {
          space_data->createNode(mat, NodeType::MATERIAL_OUTPUT);
        }

        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Shader")) {
        if (ImGui::Button("Principled BSDF")) {
          space_data->createNode(mat, NodeType::PRINCIPLED_BSDF);
        }

        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Texture")) {
				if (ImGui::Button("Brick Texture")) {
					space_data->createNode(mat, NodeType::BRICK_TEXTURE);
				}
				if (ImGui::Button("Checker Texture")) {
					space_data->createNode(mat, NodeType::CHECKER_TEXTURE);
				}
        if (ImGui::Button("Image Texture")) {
          space_data->createNode(mat, NodeType::IMAGE_TEXTURE);
        }

        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Color")) {
        if (ImGui::Button("Brightness/Contrast")) {
          space_data->createNode(mat, NodeType::BRIGHTNESS_CONTRAST);
        }
        if (ImGui::Button("Gamma")) {
          space_data->createNode(mat, NodeType::GAMMA);
        }
        if (ImGui::Button("Invert")) {
          space_data->createNode(mat, NodeType::INVERT);
        }
				if (ImGui::Button("Light Falloff")) {
					space_data->createNode(mat, NodeType::LIGHT_FALLOFF);
				}

        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Vector")) {

        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Converter")) {
        if (ImGui::Button("Mix")) {
          space_data->createNode(mat, NodeType::MIX);
        }

        ImGui::EndMenu();
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Node")) {
      if (ImGui::Button("Delete")) {
      }
      if (ImGui::Button("Duplicate")) {
      }
      if (ImGui::Button("Copy")) {
      }
      if (ImGui::Button("Paste")) {
      }

      ImGui::Separator();

      if (ImGui::Button("Resize")) {
      }
      if (ImGui::Button("Move")) {
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
	
  ImGui::End();

  // TODO: ImNodes::IsLinkDestroyed is not impelemted on the imnodes side (or
  // thats just me stupid), so check for deletion by our own
  int num_selected = ImNodes::NumSelectedLinks();
  // TODO: dont check for input in here - use update method
  if (num_selected > 0 && Input::wasKeyReleased(SDLK_DELETE)) {
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
