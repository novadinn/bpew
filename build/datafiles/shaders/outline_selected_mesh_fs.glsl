void main() {
    vec4 origColor = texture(colorTexture, vs_inout.texCoord);
    vec4 entityID = texture(entitiesTexture, vs_inout.texCoord);

    float mult = (sign(entityID.x) + 1) * 0.5f;
    int hit = int(float(entityID.x == selectedEntityID) * mult);

    fragColor = mix(origColor, vec4(outlineColor, origColor.a), mixFactor * hit);
}
