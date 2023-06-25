#ifndef SHADER_INFO_H
#define SHADER_INFO_H

#include <vector>
#include <string>
#include <set>

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
    std::vector<ShaderInOut> vins, fins;    
    std::vector<ShaderUniformBuffer> uniform_buffers;
    std::vector<ShaderInOut> vouts, fouts;    
    std::vector<ShaderUniform> uniforms;
    std::vector<ShaderUniformArray> uniform_arrays;
    std::vector<ShaderDefine> defines;
    std::vector<ShaderInterfaceInfo> interfaces;

    std::string fragment_source, vertex_source;	
    // NOTE: the difference between typedeps in that ShaderBuilder will
    // recursively search for other deps
    std::set<std::string> deps;    
    std::set<std::string> typedeps;
};


#endif
