void main() {    
    vec3 norm = normalize(vs_in.normal);
    vec3 viewDir = normalize(viewPos - vs_in.fragPos);

    vec3 result = calcDirLight(dirLight, norm, viewDir, shininess);
    
	fragColor = surface() + vec4(result, 0.0f);
    entityID = entity;
}
