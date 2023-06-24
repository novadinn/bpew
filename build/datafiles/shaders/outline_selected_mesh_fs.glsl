void main() {
    vec4 origColor = texture(colorTexture, vs_in.texCoord);
    vec4 entityID = texture(entitiesTexture, vs_in.texCoord);

    float mult = (sign(entityID.x) + 1) * 0.5f;
    int hit = int(float(entityID.x == selectedEntityID) * mult);

    fragColor = mix(origColor, vec4(outlineColor, origColor.a), mixFactor * hit);
}
