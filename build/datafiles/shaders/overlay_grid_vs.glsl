void main() {
  vs_inout.vertexPosition = vec3(aPos);

  gl_Position = projection * view * vec4(aPos, 1.0f);
  color = aColor;
}
