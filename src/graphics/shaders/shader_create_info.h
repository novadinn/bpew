#ifndef SHADER_CREATE_INFO
#define SHADER_CREATE_INFO

#include "shader_info.h"
#include "shader_builder.h"

struct ShaderCreateInfo {	
    ShaderInfo info;

    static ShaderCreateInfo create() {
	ShaderCreateInfo info;
	return info;
    }

    ShaderCreateInfo& fin(ShaderType type, uint location, std::string name) {
	ShaderInOut fin;
	fin.type = type;
	fin.location = location;
	fin.name = name;
	info.fins.push_back(fin);
	return *this;
    }

    ShaderCreateInfo& vin(ShaderType type, uint location, std::string name) {
	ShaderInOut vin;
	vin.type = type;	    
	vin.location = location;
	vin.name = name;
	info.vins.push_back(vin);
	return *this;
    }

    ShaderCreateInfo& vout(ShaderType type, uint location, std::string name) {
	ShaderInOut vout;
	vout.type = type;
	vout.location = location;
	vout.name = name;
	info.vouts.push_back(vout);
	return *this;
    }

    ShaderCreateInfo& interface(std::string type, std::string in_name,
				std::string out_name, std::string body) {
	ShaderInterface interface;
	interface.type = type;
	interface.in_name = in_name;
	interface.out_name = out_name;
	interface.body = body;
	info.interfaces.push_back(interface);
	return *this;
    }
	
    ShaderCreateInfo& fout(ShaderType type, uint location, std::string name) {
	ShaderInOut fout;
	fout.type = type;
	fout.location = location;
	fout.name = name;
	info.fouts.push_back(fout);
	return *this;
    }

    ShaderCreateInfo& fragment(std::string source) {
	info.fragment_source = source;
	return *this;
    }

    ShaderCreateInfo& vertex(std::string source) {
	info.vertex_source = source;
	return *this;
    }

    ShaderCreateInfo& uniform(ShaderType type, std::string name) {
	ShaderUniform uniform;
	uniform.type = ShaderBuilder::fromType(type);
	uniform.name = name;
	info.uniforms.push_back(uniform);
	return *this;
    }

    ShaderCreateInfo& uniform(std::string type, std::string name) {
	ShaderUniform uniform;
	uniform.type = type;
	uniform.name = name;
	info.uniforms.push_back(uniform);
	return *this;
    }

    ShaderCreateInfo& uniformArray(ShaderType type, std::string name, uint size) {
	ShaderUniformArray uniform_array;
	uniform_array.type = ShaderBuilder::fromType(type);
	uniform_array.name = name;
	uniform_array.size = size;
	info.uniform_arrays.push_back(uniform_array);
	return *this;
    }    

    ShaderCreateInfo& uniformArray(std::string type, std::string name, uint size) {
	ShaderUniformArray uniform_array;
	uniform_array.type = type;
	uniform_array.name = name;
	uniform_array.size = size;
	info.uniform_arrays.push_back(uniform_array);
	return *this;
    }

    ShaderCreateInfo& uniformBuffer(std::string name, uint binding, std::string body) {
	ShaderUniformBuffer buffer;
	buffer.name = name;	
	buffer.body = body;
	buffer.binding = binding;
	info.uniform_buffers.push_back(buffer);	
	return *this;
    }

    ShaderCreateInfo& define(std::string name, std::string value) {
	ShaderDefine define;
	define.name = name;
	define.value = value;
	info.defines.push_back(define);
	return *this;
    }

    ShaderCreateInfo& define(std::string name) {
	ShaderDefine define;	    
	define.name = name;
	info.defines.push_back(define);
	return *this;
    }

    ShaderCreateInfo& typedep(std::string source) {
	info.typedeps.insert(source);
	return *this;
    }
	
    ShaderCreateInfo& dep(std::string source) {
	info.deps.insert(source);
	return *this;
    }

    ShaderCreateInfo& removeDep(const char* dep) {
	
	for(std::set<std::string>::iterator it = info.deps.begin(); it != info.deps.end(); ++it) {
	    if(strcmp(it->c_str(), dep) == 0) {
		info.deps.erase(it);
		break;
	    }
	}
	return *this;
    }
};

#endif
