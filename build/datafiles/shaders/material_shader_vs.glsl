void main() {
  vec4 worldPosition = model * vec4(aPos, 1.0f);

  vs_inout.fragPos = vec3(worldPosition);
  vs_inout.normal = mat3(transpose(inverse(model))) * aNormal;
  vs_inout.texCoord = aTexCoord;

  gl_Position = projection * view * worldPosition;
}
