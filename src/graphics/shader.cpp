#include "shader.h"

#include "../core/log.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cstdio>

bool Shader::createFromFile(const char* vert_path, const char* frag_path) {
	long file_length;

	FILE* vert_fp;
	char* vert_content;

	if((vert_fp = fopen(vert_path, "r")) == NULL) {
		LOG_ERROR("Failed to load file at path%s\n", vert_path);
		return false;
	}
	
	fseek(vert_fp, 0, SEEK_END);
	file_length = ftell(vert_fp);
	fseek(vert_fp, 0, SEEK_SET);
	vert_content = (char*)malloc(file_length);
	fread(vert_content, 1, file_length, vert_fp);
	vert_content[file_length] = '\0';

	FILE* frag_fp;
	char* frag_content;

	if((frag_fp = fopen(frag_path, "r")) == NULL) {
		LOG_ERROR("Failed to load file at path%s\n", frag_path);
		return false;
	}
	
	fseek(frag_fp, 0, SEEK_END);
	file_length = ftell(frag_fp);
	fseek(frag_fp, 0, SEEK_SET);
	frag_content = (char*)malloc(file_length);
	fread(frag_content, 1, file_length, frag_fp);
	frag_content[file_length] = '\0';
	
	bool result = createFromSource(vert_content, frag_content);

	free(vert_content);
	free(frag_content);
	
	fclose(vert_fp);
	fclose(frag_fp);

	return result;
}

bool Shader::createFromSource(const char* vsrc, const char* fsrc) {
	// Compile the vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	const GLchar *src = (const GLchar *)vsrc;
	glShaderSource(vs, 1, &src, 0);

	glCompileShader(vs);

	GLint compp = 0;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compp);
	if(compp == GL_FALSE) {
		GLint ml = 0;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &ml);

		std::vector<GLchar> log(ml);
		glGetShaderInfoLog(vs, ml, &ml, &log[0]);

		glDeleteShader(vs);

		LOG_ERROR("Failed to load Vertex Shader. \n%s\n", &log[0]);

		return false;
	}

	// Compile the fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	src = (const GLchar *)fsrc;
	glShaderSource(fs, 1, &src, 0);

	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &compp);
	if (compp == GL_FALSE) {
		GLint ml = 0;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &ml);

		std::vector<GLchar> log(ml);
		glGetShaderInfoLog(fs, ml, &ml, &log[0]);
	
		glDeleteShader(fs);
		glDeleteShader(vs);

		LOG_ERROR("Failed to load Fragment Shader. \n%s\n", &log[0]);
	
		return false;
	}

	// Create the final shader program
	id = glCreateProgram();

	glAttachShader(id, vs);
	glAttachShader(id, fs);

	glLinkProgram(id);

	GLint linkp = 0;
	glGetProgramiv(id, GL_LINK_STATUS, (int *)&linkp);
	if (linkp == GL_FALSE) {
		GLint ml = 0;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &ml);
	    
		std::vector<GLchar> log(ml);
		glGetProgramInfoLog(id, ml, &ml, &log[0]);

		glDeleteProgram(id);
		glDeleteShader(vs);
		glDeleteShader(fs);

		LOG_ERROR("Failed to create Program. \n%s\n", &log[0]);
	
		return false;
	}
	
	glDetachShader(id, vs);
	glDetachShader(id, fs);

	return true;
}

void Shader::destroy() {
	glDeleteProgram(id);
}
    
void Shader::bind() {
	glUseProgram(id);
}

void Shader::unbind() {
	glUseProgram(0);
}

void Shader::setFloat(const char* s, float v) {
	glUniform1f(glGetUniformLocation(id, s), v);
}
    
void Shader::setVec2(const char* s, const glm::vec2& v) {
	glUniform2f(glGetUniformLocation(id, s), v.x, v.y);
}
    
void Shader::setVec3(const char* s, const glm::vec3& v) {
	glUniform3f(glGetUniformLocation(id, s), v.x, v.y, v.z);
}
    
void Shader::setVec4(const char* s, const glm::vec4& v) {
	glUniform4f(glGetUniformLocation(id, s), v.x, v.y, v.z, v.w);
}
        
void Shader::setInt(const char* s, int v) {
	glUniform1i(glGetUniformLocation(id, s), v);
}
    
void Shader::setInt2(const char* s, const glm::ivec2& v) {
	glUniform2i(glGetUniformLocation(id, s), v.x, v.y);
}
    
void Shader::setInt3(const char* s, const glm::ivec3& v) {
	glUniform3i(glGetUniformLocation(id, s), v.x, v.y, v.z);
}
    
void Shader::setInt4(const char* s, const glm::ivec4& v) {
	glUniform4i(glGetUniformLocation(id, s), v.x, v.y, v.z, v.w);
}
    
void Shader::setUInt(const char* s, uint v) {
	glUniform1ui(glGetUniformLocation(id, s), v);
}
    
void Shader::setUInt2(const char* s, const glm::uvec2& v) {
	glUniform2ui(glGetUniformLocation(id, s), v.x, v.y);
}
    
void Shader::setUInt3(const char* s, const glm::uvec3& v) {
	glUniform3ui(glGetUniformLocation(id, s), v.x, v.y, v.z);
}
    
void Shader::setUInt4(const char* s, const glm::uvec4& v) {
	glUniform4ui(glGetUniformLocation(id, s), v.x, v.y, v.z, v.w);
}    
        
void Shader::setMatrix2(const char* s, const glm::mat2& m) {
	glUniformMatrix2fv(glGetUniformLocation(id, s), 1, GL_FALSE, glm::value_ptr(m));
}
    
void Shader::setMatrix3(const char* s, const glm::mat3& m) {
	glUniformMatrix3fv(glGetUniformLocation(id, s), 1, GL_FALSE, glm::value_ptr(m));
}
    
void Shader::setMatrix4(const char* s, const glm::mat4& m) {
	glUniformMatrix4fv(glGetUniformLocation(id, s), 1, GL_FALSE, glm::value_ptr(m));
}    
    
void Shader::setMatrix2x3(const char* s, const glm::mat2x3& m) {
	glUniformMatrix2x3fv(glGetUniformLocation(id, s), 1, GL_FALSE, glm::value_ptr(m));
}
    
void Shader::setMatrix3x2(const char* s, const glm::mat3x2& m) {
	glUniformMatrix3x2fv(glGetUniformLocation(id, s), 1, GL_FALSE, glm::value_ptr(m));
}
    
void Shader::setMatrix2x4(const char* s, const glm::mat2x4& m) {
	glUniformMatrix2x4fv(glGetUniformLocation(id, s), 1, GL_FALSE, glm::value_ptr(m));
}
    
void Shader::setMatrix4x2(const char* s, const glm::mat4x2& m) {
	glUniformMatrix4x2fv(glGetUniformLocation(id, s), 1, GL_FALSE, glm::value_ptr(m));
}
    
void Shader::setMatrix3x4(const char* s, const glm::mat3x4& m) {
	glUniformMatrix3x4fv(glGetUniformLocation(id, s), 1, GL_FALSE, glm::value_ptr(m));
}
    
void Shader::setMatrix4x3(const char* s, const glm::mat4x3& m) {
	glUniformMatrix4x3fv(glGetUniformLocation(id, s), 1, GL_FALSE, glm::value_ptr(m));
}
