#ifndef SHADER_INFO_H
#define SHADER_INFO_H

#include <set>
#include <string>
#include <vector>

#include "../../core/platform.h"

enum class ShaderType {
  VEC2,
  VEC3,
  VEC4,
  MAT4,
  INT,
  FLOAT,
  SAMPLER_2D,
};

enum class InterpolationType {
  SMOOTH,
  FLAT,
  NOPERSPECTIVE,
};

enum class GeometryInType {
	POINTS,
	LINES,
	LINES_ADJACENCY,
	TRIANGLES,
	TRIANGLES_ADJACENCY,
};

enum class GeometryOutType {
	POINTS,
	LINE_STRIP,
	TRIANGLE_STRIP,
};

struct ShaderInterfaceField {
  InterpolationType inter;
  ShaderType type;
  std::string name;
};

struct ShaderInterfaceInfo {
  std::string name;
  std::string type;
  std::vector<ShaderInterfaceField> fields;
};

struct ShaderInOut {
  std::string type;

  uint location;
  std::string name;
};

struct ShaderUniform {
  std::string type;

  std::string name;
};

struct ShaderUniformArray {
  std::string type;
  std::string name;
  uint size;
};

struct ShaderUniformBuffer {
  ShaderInterfaceInfo interface;
  uint binding;
};

struct ShaderDefine {
  std::string name;
  std::string value;
};

struct ShaderInfo {
  std::vector<ShaderInOut> vins, fins, gins;	
  std::vector<ShaderUniformBuffer> uniform_buffers;
  std::vector<ShaderInOut> vouts, fouts, gouts;
  std::vector<ShaderUniform> uniforms;
  std::vector<ShaderUniformArray> uniform_arrays;
  std::vector<ShaderDefine> defines;
  std::vector<ShaderInterfaceInfo> vertex_interfaces;
	std::vector<ShaderInterfaceInfo> geometry_interfaces;

  std::string fragment_source, vertex_source, geometry_source;
  // NOTE: the difference between typedeps in that ShaderBuilder will
  // recursively search for other deps
  std::set<std::string> deps;
  std::set<std::string> typedeps;

	GeometryInType geometry_in_type;
	GeometryOutType geometry_out_type;
	uint geometry_max_vertices;	
};

#endif
