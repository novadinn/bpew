#include "shader_parser.h"

ShaderCreateInfo ShaderParser::parseCustomShader(const char* src) {
    std::ifstream fs(std::string("datafiles/shaders/") + std::string(src));

    ShaderCreateInfo info;
    if(fs.is_open()) {
	
	
	while(1) {
	    
	}
    } else {
	printf("Failed to open shader file: %s\n", src);
    }
	
    return info;
}
