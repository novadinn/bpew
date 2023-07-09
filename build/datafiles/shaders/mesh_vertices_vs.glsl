void main() {
    vs_inout.vertexID = gl_VertexID;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    /* add a small offset to avoid z-fighting */
    gl_Position.z -= 1e-4;
}
