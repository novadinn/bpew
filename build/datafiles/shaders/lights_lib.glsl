struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float intensity;
    float intensity_multiplier;
    vec3 color;
    float spot_angle;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float intensity_multiplier;
    float range;
};

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
    float intensity_multiplier;
};

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shininess) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    vec3 ambient = light.ambient;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec;
    
    return (ambient + diffuse + specular);
}

// TODO: add specular highlight
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    // TODO: these are wrong calculations
    vec3 light_direction = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
    float diffuse_intensity = max(dot(normal, light_direction), 0.0);
    float spot_factor = dot(-light_direction, normalize(light.direction));
    float spot_attenuation = smoothstep(light.spot_angle, light.spot_angle * 0.9, spot_factor);

    return light.color * diffuse_intensity * attenuation * spot_attenuation * light.intensity * light.intensity_multiplier;
}

// TODO: add specular highlight
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 light_direction = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    // NOTE: from float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
    float diffuse_intensity = max(dot(normal, light_direction), 0.0);
    float falloff = smoothstep(light.range, light.range * 0.9, distance);

    return light.color * diffuse_intensity * attenuation * falloff * light.intensity * light.intensity_multiplier;
}

// TODO: add specular highlight
vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 light_direction = normalize(-light.direction);
    float diffuse_intensity = max(dot(normal, light_direction), 0.0);

    return light.color * light.intensity * diffuse_intensity * light.intensity_multiplier;
}
