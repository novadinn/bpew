void main() {
    vec3 norm = normalize(vs_inout.normal);
    vec3 viewDir = normalize(viewPos - vs_inout.fragPos);

    vec3 result = vec3(0.0);
    for(int i = 0; i < NUM_DIR_LIGHTS; ++i) {
	result += calcDirectionalLight(dirLights[i], norm, viewDir, shininess);
    }
    for(int i = 0; i < NUM_POINT_LIGHTS; ++i) {
	result += calcPointLight(pointLights[i], norm, viewDir, shininess);
    }
    for(int i = 0; i < NUM_SPOT_LIGHTS; ++i) {
	result += calcSpotLight(spotLights[i], norm, viewDir, shininess);
    }

    fragColor = surface() + vec4(result, 0.0f);
    entityID = vec4(float(entity), 0.0f, 0.0f, 0.0f);
}
