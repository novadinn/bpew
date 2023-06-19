#ifndef SHADER_INFO_H
#define SHADER_INFO_H

#include <vector>
#include <string>

#include "../../core/platform.h"

enum class ShaderType {
    VEC2,
    VEC3,
    VEC4,
    MAT4,
    INT,
    FLOAT,
};
    
struct ShaderInOut {
    ShaderType type;

    uint location;
    std::string name;
};
   
struct ShaderInterface {
    std::string type;
    std::string in_name;
    std::string out_name;
    std::string body;
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
    std::string name;    
    std::string body;
    uint binding;
};

struct ShaderDefine {	
    std::string name;
    std::string value;
};

struct ShaderInfo {        
    std::vector<ShaderInOut> vins, fins;    
    std::vector<ShaderInterface> interfaces;
    std::vector<ShaderUniformBuffer> uniform_buffers;
    std::vector<ShaderInOut> vouts, fouts;    
    std::vector<ShaderUniform> uniforms;
    std::vector<ShaderUniformArray> uniform_arrays;
    std::vector<ShaderDefine> defines;	

    std::string fragment_source, vertex_source, compute_source;	
    // NOTE: the difference between typedeps in that ShaderBuilder will
    // recursively search for other deps
    std::vector<std::string> deps;    
    std::vector<std::string> typedeps;
};


#endif
