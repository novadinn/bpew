#include "shader_builder.h"

#include "../material.h"
#include "shader_create_info.h"
#include "../../shaders/infos/material_shader.h"

void ShaderBuilder::build_shader_from_create_info(Shader& shader, const ShaderCreateInfo& create_info, const char* additional_info) {
    std::stringstream ss;

    ss << "#version 460 core\n";
       
    for(auto& dep : create_info.info.deps) {
	proceed_source(dep.c_str());
    }

    include_libs(ss);

    for(auto& typedep : create_info.info.typedeps) {
	std::ofstream src(std::string("datafiles/shaders/") + typedep);
	    
	if(src.is_open()) {
	    ss << src.rdbuf();
	    src.close();
	} else {
	    printf("failed to open typedep: %s\n", typedep);
	}	    
    }

    for(auto& define : create_info.info.defines) {
	ss << "#define " << define.name << " " << define.value << "\n";
    }

    for(auto& uniform : create_info.info.uniforms) {
	ss << "uniform " << uniform.type << " " << uniform.name << ";\n";
    }

    for(auto& uniform_buffer : create_info.info.uniform_buffers) {
	ss << "layout(std430, binding = " << uniform_buffer.binding << ") uniform " << uniform_buffer.name << " " << ";\n";
    }    

    ss << additional_info;
    
    std::stringstream vs;

    vs << ss.str();
	       
    for(auto& vin : create_info.info.vins) {
	vs << "layout (location = " << vin.location << ") in " << from_type(vin.type) << " " << vin.name << ";\n";
    }

    for(auto& vout : create_info.info.vouts) {
	vs << "out " << from_type(vout.type) << " " << vout.name << ";\n";
    }
	
    for(auto& interface : create_info.info.interfaces) {
	vs << "out " << interface.type << " " << interface.body << " " << interface.out_name << ";\n";
    }
			
    std::ifstream file;

    file.open(std::string("datafiles/shaders/") + create_info.info.vertex_source);

    if(file.is_open()) {
	vs << file.rdbuf();
	file.close();
    } else {
	printf("failed to load vertex shader file: %s\n", create_info.info.vertex_source.c_str());
    }
	
    std::stringstream fs;

    fs << ss.str();
		
    for(auto& fin : create_info.info.fins) {
	fs << "in " << from_type(fin.type) << " " << fin.name << ";\n";
    }

    for(auto& fout : create_info.info.fouts) {
	fs << "layout (location = " << fout.location << ") out " << from_type(fout.type) << " " << fout.name << ";\n";
    }

    for(auto& interface : create_info.info.interfaces) {
	fs << "in " << interface.type << " " << interface.body << " " << interface.in_name << ";\n";
    }

    file.open(std::string("datafiles/shaders/") + create_info.info.fragment_source);

    if(file.is_open()) {
	fs << file.rdbuf();
	file.close();
    } else {
	printf("failed to load fragment shader file: %s\n", create_info.info.fragment_source.c_str());
    }		

    printf("%s\n%s\n", vs.str().c_str(), fs.str().c_str());

    shader.destroy();
    if(!shader.createFromSource(vs.str().c_str(), fs.str().c_str())) {
	printf("Failed to build shader\n");
    }
}

void ShaderBuilder::build_material_shader(Material& material) {
    Sha new_sha = generate_material_sha(material);

    if(sha.sha.size() != 0 && new_sha.sha == sha.sha) {
	return;
    }

    sha = new_sha;
    
    ShaderCreateInfo info = material_shader_create_info;

    std::stringstream ss;    
      
    for(auto& node : material.nodes) {
	if(node.type == NodeType::MATERIAL_OUTPUT) {
	    continue;
	}
	
	const char* node_name = get_node_name(node.type);
	
	std::string node_src(node_name);
	node_src.append(".glsl");
	    
	if(libs.contains(node_src)) {
	    continue; 
	}
	
	info.dep(node_src);
    }
    
    for(auto& node : material.nodes) {
	if(node.type == NodeType::MATERIAL_OUTPUT) {
	    continue;
	}
	build_node_uniforms(info, &node);       	
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
	return;
    }    
    
    const NodeProperty* surface = nullptr;
    const NodeProperty* volume = nullptr;
    const NodeProperty* displacement = nullptr;    

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

    ss << "vec4 surface() {\n";
    if(surface == nullptr || surface->links.size() == 0) {
	ss << "return vec4(0.5);\n";
    } else {	
	Node& surface_node = material.nodes[surface->links[0].output_node];
	build_node(ss, &surface_node, material);
	ss << "return output_" << surface_node.id.id << "_" << surface->links[0].output->id.id << ";\n";
    }
    ss << "}\n";
        
    build_shader_from_create_info(material.shader, info, ss.str().c_str());
}

void ShaderBuilder::build_node_uniforms(ShaderCreateInfo& info, const Node* node) {
    for(auto& input : node->inputs) {
	if(input.links.size() == 0) {
	    build_node_uniform(info, node, input);
	}
    }    
}

void ShaderBuilder::build_node_uniform(ShaderCreateInfo& info, const Node* node, const NodeProperty& prop) {
    std::string name = std::string("input_") + std::to_string(node->id.id) + std::string("_") + std::to_string(prop.id.id);

    ShaderType type;    
    switch(prop.type) {
    case NodePropertyType::COLOR:
	type = ShaderType::VEC4;
	break;
    case NodePropertyType::VECTOR3:
	type = ShaderType::VEC3;
	break;
    case NodePropertyType::VECTOR2:
	type = ShaderType::VEC2;
	break;
    case NodePropertyType::FLOAT:
	type = ShaderType::FLOAT;
	break;
    case NodePropertyType::INT:	
    case NodePropertyType::ENUM:
    case NodePropertyType::TEXTURE:
	type = ShaderType::INT;
	break;
    case NodePropertyType::SHADER:
	// TODO: what we should do?
	break;	
    default:
	printf("Unhandled node property type\n");
	return;
    }

    info.uniform(type, name);
}

void ShaderBuilder::build_node(std::stringstream& ss, const Node* node, Material& material) {        
    for(auto& input : node->inputs) {
	// TODO: should be only one link on input
	if(input.links.size() > 0) {
	    build_node(ss, &material.nodes[input.links[0].output_node], material);	
	}
    }

    for(auto& output : node->outputs) {
	ss << from_type(output.type) << " output_" << node->id.id << "_" << output.id.id << ";\n";
    }

    ss << get_node_name(node->type) << "(";
    for(auto& input : node->inputs) {
	
	ss << "input_" << node->id.id << "_" << input.id.id << ", ";
    }
    for(int i = 0; i < node->outputs.size(); ++i) {
	auto& output = node->outputs[i];
	
	ss << "output_" << node->id.id << "_" << output.id.id;

	if(i != node->outputs.size() - 1) {
	    ss << ", ";
	}
    }

    ss << ");\n";
}

const char* ShaderBuilder::get_node_name(NodeType type) {
    const char* src = "";
    switch(type) {
    case NodeType::RGB:
	src = "node_rgb";
	break;
    default:
	printf("Unknown node type: %d\n", type);	
    }

    return src;
}

const char* ShaderBuilder::from_type(NodePropertyType type) {
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

const char* ShaderBuilder::from_type(ShaderType type) {
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
    default:
	printf("unhandled type: %d\n", type);
	return "";
    }
}

void ShaderBuilder::proceed_source(const char* dep) {
    std::string lib(dep);    
    
    if(libs.contains(lib)) {
	return;
    }

    auto it = libs.insert(lib);

    std::ifstream src(std::string("datafiles/shaders/") + std::string(dep));
    
    if(src.is_open()) {
	std::string line;
	while(std::getline(src, line)) {
	    Tokenizer tokenizer(line.c_str(), line.size());

	    Token token = tokenizer.read_token();
	    
	    if(token.type == TokenType::DIRECTIVE) {
		if(token.value == "include") {
		    token = tokenizer.read_token();

		    if(token.type == TokenType::STR) {			
			proceed_source(token.value.c_str());
			continue;
		    }
		}
	    }	    
	}              	
	
	src.close();
    } else {
	libs.erase(it.first);
	printf("failed to open dep: %s\n", dep);
    }
}

void ShaderBuilder::include_libs(std::stringstream& ss) {
    for(auto& lib : libs) {
	include_lib(ss, lib.c_str());
    }
}

void ShaderBuilder::include_lib(std::stringstream& ss, const char* dep) {
    // TODO: extend filesystem to create path
    // e.g. in "dir/file" path "../shader" to "shader"
    std::ifstream src(std::string("datafiles/shaders/") + std::string(dep));

    std::string lib(dep);    
    
    if(src.is_open()) {
	std::string line;
	while(std::getline(src, line)) {
	    Tokenizer tokenizer(line.c_str(), line.size());

	    Token token = tokenizer.read_token();
	    
	    if(token.type == TokenType::DIRECTIVE) {
		if(token.value == "include") {
		    token = tokenizer.read_token();

		    if(token.type == TokenType::STR) {
			continue;
		    }
		}
	    }

	    ss << line << '\n';
	}       
	
	src.close();
    } else {
	printf("failed to open dep: %s\n", dep);
    }
}

Sha ShaderBuilder::generate_material_sha(Material& material) {
    Sha sha;

    std::stringstream material_info;

    for(auto& node : material.nodes) {	
	material_info << node.id.id;

	for(auto& input : node.inputs) {
	    material_info << input.id.id;

	    for(auto& link : input.links) {
		material_info << link.output->id.id;
		material_info << link.input->id.id;
	    }
	}

	for(auto& output : node.outputs) {
	    material_info << output.id.id;

	    for(auto& link : output.links) {
		material_info << link.output->id.id;
		material_info << link.input->id.id;
	    }
	}
    }

    std::string value = material_info.str();
    sha.create(value.c_str(), value.size());
    return sha;
}
