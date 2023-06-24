void main() {
    vec3 norm = normalize(vs_in.normal);
    vec3 viewDir = normalize(viewPos - vs_in.fragPos);

    vec3 result = calcDirLight(dirLight, norm, viewDir, shininess);

	fragColor = vec4(0.5f, 0.5f, 0.5f, 1.0f) + vec4(result, 0.0f);
    entityID = vec4(float(entity), 0.0f, 0.0f, 0.0f);
}
