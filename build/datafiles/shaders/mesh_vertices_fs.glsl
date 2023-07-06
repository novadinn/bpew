void main() {
    fragColor = vec4(color, 1.0f);
    vertexID = vec4(float(currentEntityID), float(vs_inout.vertexID), 0.0f, 0.0f);
}
