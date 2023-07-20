#ifndef SHADER_CREATE_INFO
#define SHADER_CREATE_INFO

#include "shader_builder.h"
#include "shader_info.h"

struct ShaderInterfaceCreateInfo {
  ShaderInterfaceInfo interface;

  static ShaderInterfaceCreateInfo &create(std::string type) {
    ShaderInterfaceCreateInfo *info = new ShaderInterfaceCreateInfo();
    info->interface.type = type;
    return *info;
  }

  ShaderInterfaceCreateInfo &flat(ShaderType type, std::string name) {
    ShaderInterfaceField field;
    field.inter = InterpolationType::FLAT;
    field.type = type;
    field.name = name;
    interface.fields.push_back(field);
    return *this;
  }

  ShaderInterfaceCreateInfo &noperspective(ShaderType type, std::string name) {
    ShaderInterfaceField field;
    field.inter = InterpolationType::NOPERSPECTIVE;
    field.type = type;
    field.name = name;
    interface.fields.push_back(field);
    return *this;
  }

  ShaderInterfaceCreateInfo &smooth(ShaderType type, std::string name) {
    ShaderInterfaceField field;
    field.inter = InterpolationType::SMOOTH;
    field.type = type;
    field.name = name;
    interface.fields.push_back(field);
    return *this;
  }
};

struct ShaderCreateInfo {
  ShaderInfo info;

  static ShaderCreateInfo create() {
    ShaderCreateInfo info;
    return info;
  }

  ShaderCreateInfo &fin(ShaderType type, std::string name) {
    ShaderInOut fin;
    fin.type = ShaderBuilder::fromType(type);
    fin.name = name;
    info.fins.push_back(fin);
    return *this;
  }

  ShaderCreateInfo &fin(std::string type, std::string name) {
    ShaderInOut fin;
    fin.type = type;
    fin.name = name;
    info.fins.push_back(fin);
    return *this;
  }

  ShaderCreateInfo &vin(ShaderType type, uint location, std::string name) {
    ShaderInOut vin;
    vin.type = ShaderBuilder::fromType(type);
    vin.location = location;
    vin.name = name;
    info.vins.push_back(vin);
    return *this;
  }

	ShaderCreateInfo &gin(ShaderType type, std::string name) {
    ShaderInOut gin;
    gin.type = ShaderBuilder::fromType(type);
    gin.name = name;
    info.gins.push_back(gin);
    return *this;
  }

  ShaderCreateInfo &gin(std::string type, std::string name) {
    ShaderInOut gin;
    gin.type = type;
    gin.name = name;
    info.gins.push_back(gin);
    return *this;
  }

	ShaderCreateInfo &primitive(GeometryInType in, GeometryOutType out, uint max_vertices) {
		info.geometry_in_type = in;
		info.geometry_out_type = out;
		info.geometry_max_vertices = max_vertices;
		return *this;
	} 

  ShaderCreateInfo &vout(ShaderType type, std::string name) {
    ShaderInOut vout;
    vout.type = ShaderBuilder::fromType(type);
    vout.name = name;
    info.vouts.push_back(vout);
    return *this;
  }

  ShaderCreateInfo &vout(std::string type, std::string name) {
    ShaderInOut vout;
    vout.type = type;
    vout.name = name;
    info.vouts.push_back(vout);
    return *this;
  }

  ShaderCreateInfo &fout(ShaderType type, uint location, std::string name) {
    ShaderInOut fout;
    fout.type = ShaderBuilder::fromType(type);
    fout.location = location;
    fout.name = name;
    info.fouts.push_back(fout);
    return *this;
  }

	ShaderCreateInfo &gout(ShaderType type, std::string name) {
    ShaderInOut gout;
    gout.type = ShaderBuilder::fromType(type);
    gout.name = name;
    info.gouts.push_back(gout);
    return *this;
  }

  ShaderCreateInfo &gout(std::string type, std::string name) {
    ShaderInOut gout;
    gout.type = type;
    gout.name = name;
    info.gouts.push_back(gout);
    return *this;
  }

  ShaderCreateInfo &fragment(std::string source) {
    info.fragment_source = source;
    return *this;
  }

  ShaderCreateInfo &vertex(std::string source) {
    info.vertex_source = source;
    return *this;
  }

	ShaderCreateInfo &geometry(std::string source) {
    info.geometry_source = source;
    return *this;
  }

  ShaderCreateInfo &uniform(ShaderType type, std::string name) {
    ShaderUniform uniform;
    uniform.type = ShaderBuilder::fromType(type);
    uniform.name = name;
    info.uniforms.push_back(uniform);
    return *this;
  }

  ShaderCreateInfo &uniform(std::string type, std::string name) {
    ShaderUniform uniform;
    uniform.type = type;
    uniform.name = name;
    info.uniforms.push_back(uniform);
    return *this;
  }

  ShaderCreateInfo &uniformArray(ShaderType type, std::string name, uint size) {
    ShaderUniformArray uniform_array;
    uniform_array.type = ShaderBuilder::fromType(type);
    uniform_array.name = name;
    uniform_array.size = size;
    info.uniform_arrays.push_back(uniform_array);
    return *this;
  }

  ShaderCreateInfo &uniformArray(std::string type, std::string name,
                                 uint size) {
    ShaderUniformArray uniform_array;
    uniform_array.type = type;
    uniform_array.name = name;
    uniform_array.size = size;
    info.uniform_arrays.push_back(uniform_array);
    return *this;
  }

  ShaderCreateInfo &uniformBuffer(ShaderInterfaceCreateInfo interface,
                                  std::string name, uint binding) {
    ShaderUniformBuffer buffer;
    buffer.interface = interface.interface;
    buffer.binding = binding;
    buffer.interface.name = name;
    info.uniform_buffers.push_back(buffer);
    return *this;
  }

  ShaderCreateInfo &define(std::string name, std::string value) {
    ShaderDefine define;
    define.name = name;
    define.value = value;
    info.defines.push_back(define);
    return *this;
  }

  ShaderCreateInfo &define(std::string name) {
    ShaderDefine define;
    define.name = name;
    info.defines.push_back(define);
    return *this;
  }

  ShaderCreateInfo &typedep(std::string source) {
    info.typedeps.insert(source);
    return *this;
  }

  ShaderCreateInfo &dep(std::string source) {
    info.deps.insert(source);
    return *this;
  }

  ShaderCreateInfo &ginterface(ShaderInterfaceCreateInfo interface,
                              std::string name) {
    interface.interface.name = name;
    info.geometry_interfaces.push_back(interface.interface);
    return *this;
  }

	ShaderCreateInfo &vinterface(ShaderInterfaceCreateInfo interface,
                              std::string name) {
    interface.interface.name = name;
    info.vertex_interfaces.push_back(interface.interface);
    return *this;
  }

  void removeDep(const char *dep) {
    std::string str_dep(dep);

		info.deps.erase(str_dep);
  }
};

#endif
