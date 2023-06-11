#include "shader_builder.h"

#include "shader_create_info.h"

Shader ShaderBuilder::build_shader_from_create_info(const ShaderCreateInfo& create_info) {
    std::stringstream ss;
	
    ss << "#version 460 core\n";
    
    for(auto& dep : create_info.info.deps) {
	rec_include(ss, dep.c_str());
    }

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
	
    Shader shader;
    if(!shader.createFromSource(vs.str().c_str(), fs.str().c_str())) {
	printf("Failed to build shader\n");
    }
	
    return shader;
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

void ShaderBuilder::rec_include(std::stringstream& ss, const char* dep) {
    // TODO: extend filesystem to create path
    // e.g. in "dir/file" path "../shader" to "shader"
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
			rec_include(ss, token.value.c_str());
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
