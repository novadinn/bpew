#ifndef SHADER_H
#define SHADER_H

#include "../../core/platform.h"
#include <glm/glm.hpp>

struct Shader {
  bool createFromFile(const char *vert_path, const char *frag_path, const char *geom_path = nullptr);
  bool createFromSource(const char *vsrc, const char *fsrc, const char *gsrc = nullptr);
  void destroy();

  void bind();
  void unbind();

  void setFloat(const char *s, float v);
  void setVec2(const char *s, const glm::vec2 &v);
  void setVec3(const char *s, const glm::vec3 &v);
  void setVec4(const char *s, const glm::vec4 &v);

  void setInt(const char *s, int v);
  void setInt2(const char *s, const glm::ivec2 &v);
  void setInt3(const char *s, const glm::ivec3 &v);
  void setInt4(const char *s, const glm::ivec4 &v);

  void setUInt(const char *s, uint v);
  void setUInt2(const char *s, const glm::uvec2 &v);
  void setUInt3(const char *s, const glm::uvec3 &v);
  void setUInt4(const char *s, const glm::uvec4 &v);

  void setMatrix2(const char *s, const glm::mat2 &m);
  void setMatrix3(const char *s, const glm::mat3 &m);
  void setMatrix4(const char *s, const glm::mat4 &m);

  void setMatrix2x3(const char *s, const glm::mat2x3 &m);
  void setMatrix3x2(const char *s, const glm::mat3x2 &m);
  void setMatrix2x4(const char *s, const glm::mat2x4 &m);
  void setMatrix4x2(const char *s, const glm::mat4x2 &m);
  void setMatrix3x4(const char *s, const glm::mat3x4 &m);
  void setMatrix4x3(const char *s, const glm::mat4x3 &m);

  inline uint getID() const { return id; }

private:
  uint id;
};

#endif // SHADER_H
