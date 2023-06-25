void main() {
    vec4 entityID = texture(entitiesTexture, vs_inout.texCoord);

    float mult = (sign(entityID.x) + 1) * 0.5f;
    int hit = int(float(entityID.x == selectedEntityID) * mult);

    fragColor = FxaaPixelShader(vs_inout.texCoordNoPInterp, colorTexture,
				vec2(1/viewportSize.x, 1/viewportSize.y),
				1.0, 0.125, 0.0625);

    fragColor = mix(fragColor, vec4(outlineColor, fragColor.a), mixFactor * hit);
}
