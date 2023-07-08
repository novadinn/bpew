void main() {
    float mult = (sign(currentEntityID) + 1) * 0.5f;
    int hit = int(float(currentEntityID == selectedEntityID) *
		  float(vs_inout.vertexID == selectedVertexID) *
		  mult);

    fragColor = mix(vec4(color, 1.0), vec4(selectionColor, 1.0), mixFactor * hit);
}
