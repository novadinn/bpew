#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "glm/glm.hpp"

struct DirectionalLightProperties {
  /* this is here to avoid msvc empty-struct error */
  char unused;
};

struct PointLightProperties {
  /* this is here to avoid msvc empty-struct error */
  char unused;
};

struct SpotLightProperties {
  float cut_off;
  float outer_cut_off;
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

  glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
  glm::vec3 diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
  glm::vec3 specular = glm::vec3(0.1f, 0.1f, 0.1f);
  LightProperties properties;
};

#endif // LIGHT_COMPONENT_H
