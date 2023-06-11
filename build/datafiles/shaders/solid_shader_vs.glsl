void main() {
    vec4 worldPosition = model* vec4(aPos, 1.0f);

    vs_out.fragPos = vec3(worldPosition);
    vs_out.normal = mat3(transpose(inverse(model))) * aNormal;

	gl_Position = projection * view * worldPosition;
}
