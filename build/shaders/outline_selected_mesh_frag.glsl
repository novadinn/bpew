#version 460 core

out vec4 fragColor;

in VS_OUT {
    vec2 texCoord;
} vs_in;

uniform sampler2D colorTexture;
uniform sampler2D entitiesTexture;
uniform int selectedEntityID;
uniform vec3 outlineColor;
uniform float mixFactor;

void main() {
    vec4 origColor = texture(colorTexture, vs_in.texCoord);
    vec4 entityID = texture(entitiesTexture, vs_in.texCoord);

    float mult = (sign(entityID.x) + 1) * 0.5f;
    int hit = int(float(entityID.x == selectedEntityID) * mult);

    fragColor = mix(origColor, vec4(outlineColor, origColor.a), mixFactor * hit);
}
