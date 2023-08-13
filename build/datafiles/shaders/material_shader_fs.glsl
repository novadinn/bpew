void main() {
  vec3 norm = normalize(vs_inout.normal);
  vec3 viewDir = normalize(viewPos - vs_inout.fragPos);

  vec3 result = vec3(0);
#ifdef USE_PREVIEW_SHADER
  result = calcDirectionalLight(dirLight, norm, viewDir, shininess);
#endif // USE_PREVIEW_SHADER
#ifdef USE_RENDERED_SHADER
#ifdef CAPACITY_DIR_LIGHTS
  for (int i = 0; i < num_dir_lights; ++i) {
    result += calcDirectionalLight(dirLights[i], norm, viewDir, shininess);
  }
#endif
#ifdef CAPACITY_POINT_LIGHTS
  for (int i = 0; i < num_point_lights; ++i) {
    result += calcPointLight(pointLights[i], norm, vs_inout.fragPos, viewDir,
                             shininess);
  }
#endif
#ifdef CAPACITY_SPOT_LIGHTS
  for (int i = 0; i < num_spot_lights; ++i) {
    result += calcSpotLight(spotLights[i], norm, vs_inout.fragPos, viewDir,
                            shininess);
  }
#endif
#endif // USE_RENDERED_SHADER
  fragColor = surface() + vec4(result, 0.0f);

  entityID = vec4(float(currentEntityID), 0.0f, 0.0f, 0.0f);
}
