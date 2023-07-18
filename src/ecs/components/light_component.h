#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "glm/glm.hpp"

struct DirectionalLightProperties {
  char unused;
};

struct PointLightProperties {
  float constant;
  float linear;
  float quadratic;
};

struct SpotLightProperties {
  float cut_off;
  float outer_cut_off;

  float constant;
  float linear;
  float quadratic;
};

union LightProperties {
  DirectionalLightProperties dir_light;
  PointLightProperties point_light;
  SpotLightProperties spot_light;
};

struct LightComponent {
  enum LightType {
    SPOT,
    POINT,
    DIRECTIONAL
    // TODO: area
  };

  LightType type;

  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  LightProperties properties;

  // TODO: cookie texture
};

#endif // LIGHT_COMPONENT_H
