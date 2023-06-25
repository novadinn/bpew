void main() {    
    vec3 norm = normalize(vs_inout.normal);
    vec3 viewDir = normalize(viewPos - vs_inout.fragPos);

    vec3 result = calcDirectionalLight(dirLight, norm, viewDir, shininess);
    
    fragColor = surface() + vec4(result, 0.0f);
    entityID = vec4(float(entity), 0.0f, 0.0f, 0.0f);
}
