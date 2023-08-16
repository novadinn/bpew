#include "shader_builder.h"

#include "../../core/log.h"
#include "../material.h"
#include "infos/material_shader_info.h"
#include "shader_container.h"
#include "shader_create_info.h"

void ShaderBuilder::buildDefines(std::stringstream &ss,
                                 ShaderCreateInfo &create_info) {
  ss << "#version 460 core\n";

  includeLibs(ss, create_info.info.deps);
  // TODO: remove typedeps
  // includeLibs(ss, create_info.info.typedeps);

  for (auto &define : create_info.info.defines) {
    ss << "#define " << define.name << " " << define.value << "\n";
  }

  for (auto &uniform : create_info.info.uniforms) {
    ss << "uniform " << uniform.type << " " << uniform.name << ";\n";
  }

  for (auto &uniform_buffer : create_info.info.uniform_buffers) {
    ss << "layout(std140, binding = " << uniform_buffer.binding << ") uniform ";
    buildInterface(ss, uniform_buffer.interface);
    ss << ";\n";
  }

  for (auto &uniform_array : create_info.info.uniform_arrays) {
    ss << "uniform " << uniform_array.type << " " << uniform_array.name << "["
       << uniform_array.size << "];\n";
  }
}

void ShaderBuilder::buildVertexShaderDefines(std::stringstream &vs,
                                             ShaderCreateInfo &create_info) {
  for (auto &vin : create_info.info.vins) {
    vs << "layout (location = " << vin.location << ") in " << vin.type << " "
       << vin.name << ";\n";
  }

  for (auto &vout : create_info.info.vouts) {
    vs << "out " << vout.type << " " << vout.name << ";\n";
  }

  for (auto &interface : create_info.info.vertex_interfaces) {
    vs << "out ";
    buildInterface(vs, interface);
    vs << ";\n";
  }
}

void ShaderBuilder::buildFragmentShaderDefines(std::stringstream &fs,
                                               ShaderCreateInfo &create_info) {
  for (auto &fin : create_info.info.fins) {
    fs << "in " << fin.type << " " << fin.name << ";\n";
  }

  for (auto &fout : create_info.info.fouts) {
    fs << "layout (location = " << fout.location << ") out " << fout.type << " "
       << fout.name << ";\n";
  }

  for (auto &interface : create_info.info.geometry_interfaces) {
    fs << "in ";
    buildInterface(fs, interface);
    fs << ";\n";
  }

  for (auto &interface : create_info.info.vertex_interfaces) {
    fs << "in ";
    buildInterface(fs, interface);
    fs << ";\n";
  }
}

void ShaderBuilder::buildGeometryShaderDefines(std::stringstream &gs,
                                               ShaderCreateInfo &create_info) {
  gs << "layout(" << fromType(create_info.info.geometry_in_type) << ") in;\n";
  gs << "layout(" << fromType(create_info.info.geometry_out_type)
     << ", max_vertices = " << create_info.info.geometry_max_vertices
     << ") out;\n";

  for (auto &gin : create_info.info.gins) {
    gs << "in " << gin.type << " " << gin.name << "[];\n";
  }

  for (auto &gout : create_info.info.gouts) {
    gs << "out " << gout.type << " " << gout.name << "[];\n";
  }

  for (auto &interface : create_info.info.vertex_interfaces) {
    gs << "in ";
    buildInterface(gs, interface);
    gs << "[];\n";

    gs << "out ";
    buildInterface(gs, interface);
    gs << "_fs;\n";
  }

  for (auto &interface : create_info.info.geometry_interfaces) {
    gs << "out ";
    buildInterface(gs, interface);
    gs << ";\n";
  }
}

bool ShaderBuilder::buildShaderFromCreateInfo(Shader &shader,
                                              const ShaderCreateInfo &info) {
  ShaderCreateInfo create_info = info;

  std::stringstream ss;

  buildDefines(ss, create_info);

  std::stringstream vs, fs, gs;
  const char *base_dir = "datafiles/shaders/";

  std::string buf;
  char build_result = 0x0;
  if (create_info.info.vertex_source.size() > 0) {
    vs << ss.str();
    buildVertexShaderDefines(vs, create_info);
    std::string path(base_dir);
    path.append(create_info.info.vertex_source.c_str());
    build_result |= Utils::readFile(path.c_str(), buf);
    vs << buf;
  }
  if (create_info.info.fragment_source.size() > 0) {
    fs << ss.str();
    buildFragmentShaderDefines(fs, create_info);
    std::string path(base_dir);
    path.append(create_info.info.fragment_source.c_str());
    build_result |= Utils::readFile(path.c_str(), buf) << 1;
    fs << buf;
  }
  if (create_info.info.geometry_source.size() > 0) {
    gs << ss.str();
    buildGeometryShaderDefines(gs, create_info);
    std::string path(base_dir);
    path.append(create_info.info.geometry_source.c_str());
    build_result |= Utils::readFile(path.c_str(), buf) << 2;
    gs << buf;
  }

  if (build_result == 0x7) {
    shader.destroy();

    if (!shader.createFromSource(vs.str().c_str(), fs.str().c_str(),
                                 gs.str().c_str())) {
      LOG_ERROR("Failed to build shader\n");
      return false;
    }
  } else if (build_result == 0x3) {
    shader.destroy();

    if (!shader.createFromSource(vs.str().c_str(), fs.str().c_str())) {
      LOG_ERROR("Failed to build shader\n");
      return false;
    }
  } else {
    LOG_ERROR("Failed to build shader\n");
    return false;
  }

  return true;
}

const char *ShaderBuilder::fromType(ShaderType type) {
  switch (type) {
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
    LOG_ERROR("unhandled type: %d\n", (int)type);
    return "";
  }
}

const char *ShaderBuilder::fromType(InterpolationType type) {
  switch (type) {
  case InterpolationType::NOPERSPECTIVE:
    return "noperspective";
  case InterpolationType::SMOOTH:
    return "smooth";
  case InterpolationType::FLAT:
    return "flat";
  default:
    LOG_ERROR("unhandled type: %d\n", (int)type);
    return "";
  }
}

const char *ShaderBuilder::fromType(GeometryInType type) {
  switch (type) {
  case GeometryInType::POINTS:
    return "points";
  case GeometryInType::LINES:
    return "lines";
  case GeometryInType::LINES_ADJACENCY:
    return "lines_adjacency";
  case GeometryInType::TRIANGLES:
    return "triangles";
  case GeometryInType::TRIANGLES_ADJACENCY:
    return "triangles_adjacency";
  default:
    LOG_ERROR("unhandled type %d\n", (int)type);
    return "";
  }
}

const char *ShaderBuilder::fromType(GeometryOutType type) {
  switch (type) {
  case GeometryOutType::POINTS:
    return "points";
  case GeometryOutType::LINE_STRIP:
    return "line_strip";
  case GeometryOutType::TRIANGLE_STRIP:
    return "triangle_strip";
  default:
    LOG_ERROR("unhandled type %d\n", (int)type);
    return "";
  }
}

void ShaderBuilder::includeLibs(std::stringstream &ss,
                                std::set<std::string> &libs) {
  std::set<std::string> included_libs;
  for (auto &lib : libs) {
    includeLib(ss, lib.c_str(), included_libs);
  }
}

bool ShaderBuilder::includeLib(std::stringstream &ss, const char *dep,
                               std::set<std::string> &included_libs) {
  std::string lib(dep);

  if (included_libs.contains(lib)) {
    return true;
  }

  auto it = included_libs.insert(lib);

  // TODO: extend filesystem to create path
  // e.g. in "dir/file" path "../shader" to "shader"
  std::ifstream src(Utils::joinPath("datafiles/shaders/") + std::string(dep));

  if (src.is_open()) {
    std::string line;
    while (std::getline(src, line)) {
      Tokenizer tokenizer(line.c_str(), line.size());

      Token token = tokenizer.readToken();

      if (token.type == TokenType::DIRECTIVE) {
        if (token.value == "include") {
          token = tokenizer.readToken();

          if (token.type == TokenType::STR) {
            includeLib(ss, token.value.c_str(), included_libs);
          }

          continue;
        }
      }

      ss << line << '\n';
    }

    src.close();
  } else {
    included_libs.erase(it.first);
    LOG_ERROR("failed to open dep: %s\n", dep);
    return false;
  }

  return true;
}

void ShaderBuilder::buildInterface(std::stringstream &ss,
                                   ShaderInterfaceInfo &interface_info) {
  ss << interface_info.type << " "
     << "{\n";

  for (auto &field : interface_info.fields) {
    ss << fromType(field.inter) << " " << fromType(field.type) << " "
       << field.name << ";\n";
  }

  ss << "} " << interface_info.name;
}
