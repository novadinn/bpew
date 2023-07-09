#include "shader_builder.h"

#include "../material.h"
#include "shader_create_info.h"
#include "shader_container.h"
#include "infos/material_shader_info.h"

std::map<std::string, ShaderContainer> ShaderBuilder::shaders;    

bool ShaderBuilder::buildShaderFromCreateInfo(Shader& shader, const ShaderCreateInfo& info, const char* additional_info) {
    ShaderCreateInfo create_info = info;

    std::stringstream ss;

    ss << "#version 460 core\n";
       
    for(auto& dep : create_info.info.deps) {
	proceedSource(dep.c_str(), create_info);
    }

    includeLibs(ss, create_info);

    for(auto& define : create_info.info.defines) {
	ss << "#define " << define.name << " " << define.value << "\n";
    }

    for(auto& uniform : create_info.info.uniforms) {
	ss << "uniform " << uniform.type << " " << uniform.name << ";\n";
    }

    for(auto& uniform_buffer : create_info.info.uniform_buffers) {
	ss << "layout(std140, binding = " << uniform_buffer.binding << ") uniform ";
	buildInterface(ss, uniform_buffer.interface);
    }

    for(auto& uniform_array : create_info.info.uniform_arrays) {
	ss << "uniform " << uniform_array.type << " " << uniform_array.name << "[" << uniform_array.size << "];\n";
    }
   
    std::stringstream vs;

    vs << ss.str();
	       
    for(auto& vin : create_info.info.vins) {
	vs << "layout (location = " << vin.location << ") in " << vin.type << " " << vin.name << ";\n";
    }

    for(auto& vout : create_info.info.vouts) {
	vs << "out " << vout.type << " " << vout.name << ";\n";
    }

    for(auto& interface : create_info.info.interfaces) {
	vs << "out ";
	buildInterface(vs, interface);
    }

    vs << additional_info;

    std::ifstream file;

    file.open(std::string("datafiles/shaders/") + create_info.info.vertex_source);

    if(file.is_open()) {
	vs << file.rdbuf();
	file.close();
    } else {
	printf("failed to load vertex shader file: %s\n", create_info.info.vertex_source.c_str());
	return false;
    }
	
    std::stringstream fs;

    fs << ss.str();
		
    for(auto& fin : create_info.info.fins) {
	fs << "in " << fin.type << " " << fin.name << ";\n";
    }

    for(auto& fout : create_info.info.fouts) {
	fs << "layout (location = " << fout.location << ") out " << fout.type << " " << fout.name << ";\n";
    }

    for(auto& interface : create_info.info.interfaces) {
	fs << "in ";
	buildInterface(fs, interface);
    }

    fs << additional_info;
    
    file.open(std::string("datafiles/shaders/") + create_info.info.fragment_source);

    if(file.is_open()) {
	fs << file.rdbuf();
	file.close();
    } else {
	printf("failed to load fragment shader file: %s\n", create_info.info.fragment_source.c_str());
	return false;
    }

    shader.destroy();
    if(!shader.createFromSource(vs.str().c_str(), fs.str().c_str())) {
	printf("Failed to build shader\n");
	return false;
    }

    return true;
}

void ShaderBuilder::buildMaterialShader(Material& material) {
    Sha new_sha = generateMaterialSha(material);

    if(material.shader_container && material.shader_container->sha.sha.size() != 0 && new_sha.sha == material.shader_container->sha.sha) {
	return;
    }

    if(getShaderContainer(new_sha.sha)) {
	ShaderContainer& shader = shaders[new_sha.sha];
	increaseUsage(shader);	
	if(material.shader_container) {
	    decreaseUsage(new_sha.sha, (*material.shader_container));
	}
	material.shader_container = &shader;
	return;
    }

    ShaderCreateInfo create_info = material_shader_create_info;

    std::stringstream ss;
    
    buildNodeTree(ss, create_info, material);
    create_info.define("USE_PREVIEW_SHADER");
        
    ShaderContainer container;
    container.additional_info = ss.str();
    container.create_info = create_info;
    container.users = 1;
    container.sha = new_sha;
    const auto [it, suc] = shaders.insert({new_sha.sha, container});
    material.shader_container = &it->second;

    buildShaderFromShaderContainer(material.shader_container);
}

void ShaderBuilder::buildShaderFromShaderContainer(ShaderContainer* shader_container) {
    if(buildShaderFromCreateInfo(
	   shader_container->shader,
	   shader_container->create_info,
	   shader_container->additional_info.c_str())) {
	shader_container->compiled = true;
    }    
}

void ShaderBuilder::buildMaterialRenderedShader(Material& material, uint num_spot_lights,
							 uint num_point_lights, uint num_dir_lights) {
    Sha new_sha = generateMaterialRenderedSha(material, num_spot_lights, num_point_lights, num_dir_lights);

    if(material.shader_container && material.shader_container->sha.sha.size() != 0 && new_sha.sha == material.shader_container->sha.sha) {
	return;
    }

    if(shaders.contains(new_sha.sha)) {
	ShaderContainer& shader = shaders[new_sha.sha];
	increaseUsage(shader);
	if(material.shader_container) {
	    decreaseUsage(new_sha.sha, (*material.shader_container));
	}
	material.shader_container = &shader;
	return;
    } 
    
    ShaderCreateInfo create_info = material_shader_create_info;

    std::stringstream ss;

    buildNodeTree(ss, create_info, material);
        
    create_info.dep("lights_lib.glsl");
    create_info.define("USE_RENDERED_SHADER");
    if(num_spot_lights > 0) {
	create_info.define("NUM_SPOT_LIGHTS", std::to_string(num_spot_lights));
	create_info.uniformArray("SpotLight", "spotLights", num_spot_lights);
    }
    if(num_point_lights > 0) {
	create_info.define("NUM_POINT_LIGHTS", std::to_string(num_point_lights));
	create_info.uniformArray("PointLight", "pointLights", num_point_lights);
    }
    if(num_dir_lights > 0) {
	create_info.define("NUM_DIR_LIGHTS", std::to_string(num_dir_lights));
	create_info.uniformArray("DirectionalLight", "dirLights", num_dir_lights);
    }
    
    ShaderContainer container;
    container.additional_info = ss.str();
    container.create_info = create_info;
    container.users = 1;
    container.sha = new_sha;
    const auto [it, suc] = shaders.insert({new_sha.sha, container});
    material.shader_container = &it->second;

    buildShaderFromShaderContainer(material.shader_container);
}

void ShaderBuilder::buildNodeUniforms(ShaderCreateInfo& info, Node* node) {
    for(auto& input : node->inputs) {
	if(input.source == NodePropertySource::UNIFORM) {
	    buildNodeUniform(info, node, input);	
	}
    }    
}

ShaderContainer* ShaderBuilder::getShaderContainer(std::string& hash) {
    if(!shaders.contains(hash)) {
	return nullptr;
    }

    return &shaders[hash];
}

void ShaderBuilder::buildNodeTree(std::stringstream& ss, ShaderCreateInfo& create_info, Material& material) {
    std::vector<int> prev_ids;    
    int id = 0;
    
    // generate ids
    // NOTE: We don't need to regenerate shader when only ids have changed
    for(auto& node : material.nodes) {
	prev_ids.push_back(node.id.id);
	node.id.id = id++;

	for(auto& input : node.inputs) {
	    prev_ids.push_back(input.id.id);
	    input.id.id = id++;
	}

	for(auto& output : node.outputs) {
	    prev_ids.push_back(output.id.id);
	    output.id.id = id++;
	}
    }

    for(auto& node : material.nodes) {	
	if(node.type == NodeType::MATERIAL_OUTPUT) {
	    continue;
	}
	
	const char* node_name = getNodeName(node.type);
	
	std::string node_src(node_name);
	node_src.append(".glsl");
	
	create_info.dep(node_src);
    }
    
    for(auto& node : material.nodes) {
	if(node.type == NodeType::MATERIAL_OUTPUT) {
	    continue;
	}
	buildNodeUniforms(create_info, &node);       	
    }
    
    const Node* out = nullptr;

    for(auto& node : material.nodes) {       
	if(node.type == NodeType::MATERIAL_OUTPUT) {
	    out = &node;	    
	    break;
	}
    }

    if(!out) {
	printf("WARNING: material \"%s\" output not found\n", material.name.c_str());	
    }    
    
    const NodeInput* surface = nullptr;
    const NodeInput* volume = nullptr;
    const NodeInput* displacement = nullptr;    

    if(out) {
	for(auto& input : out->inputs) {	
	    if(input.id.name == "Surface") {
		surface = &input;
	    }
	    if(input.id.name == "Volume") {
		volume = &input;
	    }
	    if(input.id.name == "Displacement") {
		displacement = &input;
	    }
	}
    }

    ss << "vec4 surface() {\n";
    if(surface == nullptr || !surface->link) {
	ss << "return vec4(0.5, 0.5, 0.5, 1.0);\n";
    } else {	
	Node* surface_node = &material.nodes[surface->link->output_node];
	buildNode(ss, surface_node, material);
	ss << "return output_" << surface_node->outputs[surface->link->output].id.id << ";\n";
    }
    ss << "}\n";

    // revert ids
    for(auto& node : material.nodes) {	
	node.id.id = prev_ids[node.id.id];

	for(auto& input : node.inputs) {	 
	    input.id.id = prev_ids[input.id.id];
	}

	for(auto& output : node.outputs) {	    
	    output.id.id = prev_ids[output.id.id];
	}
    }
}

void ShaderBuilder::buildNodeUniform(ShaderCreateInfo& info, Node* node, const NodeInput& prop) {
    std::string name = std::string("input_") + std::to_string(prop.id.id);

    ShaderType type = toType(prop.type);    
    
    info.uniform(type, name);
}

ShaderType ShaderBuilder::toType(NodePropertyType type) {
    switch(type) {
    case NodePropertyType::COLOR:
	return ShaderType::VEC4;	
    case NodePropertyType::VECTOR3:
	return ShaderType::VEC3;	
    case NodePropertyType::VECTOR2:
	return ShaderType::VEC2;	
    case NodePropertyType::FLOAT:
	return ShaderType::FLOAT;	
    case NodePropertyType::INT:	
    case NodePropertyType::ENUM:
	return ShaderType::INT;	
    case NodePropertyType::TEXTURE:
	return ShaderType::SAMPLER_2D;
    case NodePropertyType::SHADER:
	// TODO: what we should do?	
    default:	
	printf("Unhandled node property type\n");
	return ShaderType::INT;
    }
}

void ShaderBuilder::buildNode(std::stringstream& ss, Node* node, Material& material) {        
    for(auto& input : node->inputs) {
	// TODO: should be only one link on input
	if(input.link) {
	    buildNode(ss, &material.nodes[input.link->output_node], material);		
	}

	if(input.source == NodePropertySource::ATTR && !input.link) {
	    ss << fromType(input.type) << " input_" << input.id.id << ";\n";
	}
    }

    for(int i = 0; i < node->outputs.size(); ++i) {
	auto& output = node->outputs[i];
	ss << fromType(output.type) << " output_" << output.id.id << ";\n";
    }

    ss << getNodeName(node->type) << "(";
    for(int i = 0; i < node->inputs.size(); ++i) {	
	auto& input = node->inputs[i];

	switch(input.source) {
	case UNIFORM:
	    ss << "input_" << input.id.id;
	    break;
	case ATTR:
	    if(input.link)
		ss << "output_" << material.nodes[input.link->output_node].outputs[input.link->output].id.id;
	    else
		ss << "input_" << input.id.id;
	    break;
	case VS_OUT:
	    ss << "vs_inout." << input.id.name;
	    break;	
	}

	ss << ", ";	    
    }
    for(int i = 0; i < node->outputs.size(); ++i) {
	auto& output = node->outputs[i];
	
	ss << "output_" << output.id.id;

	if(i != node->outputs.size() - 1) {
	    ss << ", ";
	}
    }

    ss << ");\n";
}

const char* ShaderBuilder::getNodeName(NodeType type) {
    const char* src = "";
    switch(type) {
    case NodeType::RGB:
	src = "node_rgb";
	break;
    case NodeType::IMAGE_TEXTURE:
	src = "node_image_texture";
	break;
    case NodeType::TEXTURE_COORDINATE:
	src = "node_texture_coordinate";
	break;
    default:
	printf("Unknown node type: %d\n", type);	
    }

    return src;
}

const char* ShaderBuilder::fromType(NodePropertyType type) {
    switch(type) {
    case NodePropertyType::COLOR:
	return "vec4";
    case NodePropertyType::VECTOR3:
	return "vec3";
    case NodePropertyType::VECTOR2:
	return "vec2";
    case NodePropertyType::FLOAT:
	return "float";
    case NodePropertyType::INT:
    case NodePropertyType::ENUM:
	return "int";	
    case NodePropertyType::SHADER:
	return "";
    case NodePropertyType::TEXTURE:
	return "sampler2D";
    default:
	printf("Unknown node property type: %d\n", type);
	return "";
    }
}

const char* ShaderBuilder::fromType(ShaderType type) {
    switch(type) {
    case ShaderType::VEC2:
	return "vec2";
    case ShaderType::VEC3:
	return "vec3";
    case ShaderType::VEC4:
	return "vec4";
    case ShaderType::MAT4:
	return "mat4";
    case ShaderType::INT:
	return "int";
    case ShaderType::FLOAT:
	return "float";
    case ShaderType::SAMPLER_2D:
	return "sampler2D";
    default:
	printf("unhandled type: %d\n", type);
	return "";
    }
}

const char* ShaderBuilder::fromType(InterpolationType type) {
    switch(type) {
    case InterpolationType::NOPERSPECTIVE:
	return "noperspective";
    case InterpolationType::SMOOTH:
	return "smooth";
    case InterpolationType::FLAT:
	return "flat";
    default:
	printf("unhandled type: %d\n", type);
	return "";
    }
}

void ShaderBuilder::proceedSource(const char* dep, ShaderCreateInfo& create_info) {
    std::string lib(dep);
    
    if(create_info.info.deps.contains(lib)) {
	return;
    }

    create_info.dep(lib);

    std::ifstream src(std::string("datafiles/shaders/") + std::string(dep));
    
    if(src.is_open()) {
	std::string line;
	while(std::getline(src, line)) {
	    Tokenizer tokenizer(line.c_str(), line.size());

	    Token token = tokenizer.readToken();
	    
	    if(token.type == TokenType::DIRECTIVE) {
		if(token.value == "include") {
		    token = tokenizer.readToken();

		    if(token.type == TokenType::STR) {			
			proceedSource(token.value.c_str(), create_info);			
		    }

		    continue;
		}
	    }	    
	}              	
	
	src.close();
    } else {
	create_info.removeDep(lib.c_str());
	printf("failed to open dep: %s\n", dep);
    }
}

void ShaderBuilder::includeLibs(std::stringstream& ss, ShaderCreateInfo& create_info) {
    for(auto& lib : create_info.info.typedeps) {
	includeLib(ss, lib.c_str());
    }
    for(auto& lib : create_info.info.deps) {
	includeLib(ss, lib.c_str());
    }
}

void ShaderBuilder::includeLib(std::stringstream& ss, const char* dep) {
    // TODO: extend filesystem to create path
    // e.g. in "dir/file" path "../shader" to "shader"
    std::ifstream src(Utils::joinPath("./datafiles/shaders/") + std::string(dep));

    std::string lib(dep);    
    
    if(src.is_open()) {
	std::string line;
	while(std::getline(src, line)) {
	    Tokenizer tokenizer(line.c_str(), line.size());

	    Token token = tokenizer.readToken();
	    
	    if(token.type == TokenType::DIRECTIVE) {
		if(token.value == "include") {       
		    continue;		    
		}
	    }

	    ss << line << '\n';
	}       
	
	src.close();
    } else {
	printf("failed to open dep: %s\n", dep);
    }
}

Sha ShaderBuilder::generateMaterialSha(Material& material) {
    Sha sha;

    std::stringstream material_info;

    // TODO: sort nodes
    for(auto& node : material.nodes) {	
	material_info << (int)node.type;

	for(auto& input : node.inputs) {
	    if(input.link) {
		material_info << input.link->output;
		material_info << (int)material.nodes[input.link->output_node].type;
		material_info << (int)material.nodes[input.link->input_node].type;		
	    }		    	    
	}
    }

    std::string value = material_info.str();
    sha.create(value.c_str(), value.size());
    return sha;
}

Sha ShaderBuilder::generateMaterialRenderedSha(Material& material, uint num_spot_lights,
					       uint num_point_lights, uint num_dir_lights) {
    Sha sha = generateMaterialSha(material);
      
    std::stringstream ss;
    ss << sha.sha << num_spot_lights << num_point_lights << num_dir_lights;

    std::string value = ss.str();
    
    sha.create(value.c_str(), value.size());
    return sha;
}

void ShaderBuilder::buildInterface(
    std::stringstream& ss, ShaderInterfaceInfo& interface_info) {
    ss << interface_info.type << " " << "{\n";

    for(auto& field : interface_info.fields) {
	ss << fromType(field.inter) << " " << fromType(field.type) << " " << field.name << ";\n";
    }

    ss << "} " << interface_info.name << ";\n";
}

void ShaderBuilder::decreaseUsage(std::string& key, ShaderContainer& container) {
    container.users--;
    if(container.users == 0) {
	// TODO: clear unused shaders
	// if(container.compiled) {	    
	//     container.shader.destroy();
	// }

	// shaders.erase(key);
    }
}

void ShaderBuilder::increaseUsage(ShaderContainer& container) {
    container.users++;
}
